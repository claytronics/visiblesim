#include "map.h"
#include "localTupleSpace.hpp"
#include "tuple.hpp"
#include "contextTuple.hpp"

#include "catoms2DWorld.h"
#include "vector3D.h"

//#define VIRTUAL_COORDINATES
#define REAL_COORDINATES
//#define MAP_DEBUG

using namespace Catoms2D;

Coordinate Map::ccth;
bool Map::isConnected = false;

Map::Map(Catoms2DBlock* host): border(host) {
  connectedToHost = false;
  waiting = 0;
  toHost = NULL;
  positionKnown = false;
  catom2D = host;
}

Map::Map(const Map &m): border(m.border) {
  connectedToHost = m.connectedToHost;
  waiting = m.waiting;
  toHost = m.toHost;
  positionKnown = m.positionKnown;
  catom2D = m.catom2D;
}

Map::~Map() {}

void Map::connectToHost() {
  if(!isConnected){
    isConnected = true;
    connectedToHost = true;
    toHost = NULL;

    cout << "@" << catom2D->blockId << " is connected to host" << endl;
#ifdef VIRTUAL_COORDINATES
    // virtual coordinate
    Coordinate c = Coordinate(0,0);
    ccth.x = catom2D->position[0];
    ccth.y = catom2D->position[2];
#endif

#ifdef REAL_COORDINATES
    // real coordinate
    Coordinate c(catom2D->position[0], catom2D->position[2]);
    ccth.x = 0;
    ccth.y = 0;
#endif

    setPosition(c);
    catom2D->setColor(RED);
    buildMap();
  }
}

bool Map::handleMessage(MessagePtr message) {
  stringstream info;
  P2PNetworkInterface * recv_interface = message->destinationInterface;
  switch(message->type) {
  case GO_MAP_MSG: {
    GoMapMessage_ptr m = std::static_pointer_cast<GoMapMessage>(message);
    if (!positionKnown) {
      toHost = recv_interface;
      Coordinate c = m->getPosition(); //getPosition(toHost, m->getLast());
      setPosition(c);
#ifdef MAP_DEBUG
      Coordinate p;
      p.x = catom2D->position[0];
      p.y = catom2D->position[2];
      Coordinate real =  real2Virtual(p,ccth);
      cout << "@" << catom2D->blockId <<  " position " << position << " vs " << real << "(diff: " << position.x - real.x << "," <<  position.y - real.y << ")" << endl;
      if( real != position) { // not relevant (odd/even line of the leader)
    catom2D->setColor(BLUE);
      }
#endif

#ifdef GEO_ROUTING_TEST
#ifdef TEST_GEO_ROUTING_ONE_TO_ONE
      Coordinate src(5,1);
      Coordinate dest(5,4);
      cout << "@" << catom2D->blockId << " " << position << endl;
      if (position == src) {
    catom2D->setColor(BLUE);
    cout << "sending from " << src << " to " << dest << endl;
    //out(new ContextTuple(dest, string("testGeoRoutingOneToOne")));
      }
      if (position == dest) {
    catom2D->setColor(GREEN);
      }
#endif

#endif
      waiting = 0;
      buildMap();
      if (waiting==0) {
    mapBuilt(toHost);
      }
    } else {
      mapBuilt(recv_interface);
    }
  }
    break;
  case BACK_MAP_MSG: {
    BackMapMessage_ptr m = std::static_pointer_cast<BackMapMessage>(message);
    waiting--;
#ifdef MAP_DEBUG
    //cout << "@" << catom2D->blockId <<  " back msg " << waiting << endl;
#endif
    if (!waiting) {
      if (!connectedToHost) {
    mapBuilt(toHost);
      }
      return true;
    }
  }
    break;
  default:
    return false;
  }
  return false;
}

void Map::buildMap() {
  P2PNetworkInterface *p2p;
  for (int i=0; i<6; i++) {
    p2p = catom2D->getInterface((NeighborDirection::Direction)i);
    if( (p2p == toHost) || !p2p->connectedInterface) {
      continue;
    }
    GoMapMessage * msg = new GoMapMessage(getPosition(p2p));
    p2p->send(msg);
    waiting++;
  }
}

void Map::mapBuilt(P2PNetworkInterface *d) {
  BackMapMessage * msg = new BackMapMessage();
  d->send(msg);
}

void Map::setPosition(Coordinate p) {
  position = p;
  positionKnown = true;
}

Coordinate Map::getPosition() {
  return position;
}

Coordinate Map::real2Virtual(Coordinate p) {
  return real2Virtual(ccth,p);
}

Coordinate Map::virtual2Real(Coordinate p) {
  return virtual2Real(ccth,p);
}

Coordinate Map::real2Virtual(Coordinate o, Coordinate p) {
  Coordinate real = p;
  real.x -= o.x;
  real.y -= o.y;

  if ( (o.y%2) == 1) {
    if ((p.y%2) == 0) {
      real.x--;
    }
  }

  return real;
}

Coordinate Map::virtual2Real(Coordinate o, Coordinate p) {
  Coordinate vir = p;
  vir.x += o.x;
  vir.y += o.y;

  if ( (o.y%2) == 1) {
    if ((p.y%2) == 0) {
      vir.x++;
    }
  }
  return vir;
}

Coordinate Map::getPosition(Catoms2D::Catoms2DBlock* catom2D, Coordinate p, P2PNetworkInterface *it) {

  switch(catom2D->getDirection(it)) {
  case NeighborDirection::BottomLeft:
    if ((abs(p.y)%2) == 0) {
      p.x--;
    }
    p.y--;
    break;
  case NeighborDirection::Left:
    p.x--;
    break;
  case NeighborDirection::TopLeft:
    if ((abs(p.y)%2) == 0) {
      p.x--;
    }
    p.y++;
    break;
  case NeighborDirection::TopRight:
    if ((abs(p.y)%2) == 1) {
      p.x++;
    }
    p.y++;
    break;
  case NeighborDirection::Right:
    p.x++;
    break;
  case NeighborDirection::BottomRight:
    if ((abs(p.y)%2) == 1) {
      p.x++;
    }
    p.y--;
    break;
  }
  return p;
}

Coordinate Map::getPosition(P2PNetworkInterface *it) {
  return getPosition(catom2D, position, it);
}

P2PNetworkInterface* Map::getClosestInterface(Coordinate dest, P2PNetworkInterface *ignore) {
  P2PNetworkInterface *closest = NULL;
  int minDistance = distance(dest);
  for (int i = 0; i<6; i++) {
    P2PNetworkInterface *it = catom2D->getInterface((NeighborDirection::Direction)i);
    if((it == ignore) || !it->connectedInterface) {
      continue;
    }
    int d = distance(getPosition(it), dest);
    if (d < minDistance) {
      closest = it;
    }
  }
  return closest;
}

int Map::distance(Coordinate p1, Coordinate p2) {
  return abs(p2.x - p1.x) +  abs(p2.y - p1.y);
}

int Map::distance(Coordinate p2) {
  return distance(position,p2);
}

bool Map::areNeighbors(Coordinate p1, Coordinate p2) {
  Cell3DPosition pos1 = Cell3DPosition(p1.x, 0, p1.y);
  Cell3DPosition pos2 = Cell3DPosition(p2.x, 0, p2.y);
  return Catoms2DWorld::getWorld()->areNeighborsGridPos(pos1,pos2);
}

bool Map::isInTarget(Coordinate p) {
  Catoms2DWorld *world = Catoms2DWorld::getWorld();
  return (world->getTargetGrid(p.x,0,p.y) == fullCell);
}

P2PNetworkInterface* Map::getOnBorderNeighborInterface(RelativeDirection::Direction d) {
  return border.getInterface(d);
}

Catoms2DBlock* Map::getOnBorderNeighbor(Catoms2D::RelativeDirection::Direction d) {
  P2PNetworkInterface* p2p = getOnBorderNeighborInterface(d);
  if (!p2p->connectedInterface) {
    cerr << "error: no catom connected on border interface..." << endl;
    return NULL;
  }
  return (Catoms2DBlock*) p2p->connectedInterface->hostBlock;
}

Coordinate Map::getOnBorderNeighborCoordinate(RelativeDirection::Direction d) {
  return getPosition(border.getInterface(d));
}
