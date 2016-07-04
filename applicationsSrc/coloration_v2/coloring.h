#ifndef COLORING_H_
#define COLORING_H_

#include <list>
#include <map>
#include <memory>
#include "network.h"
#include "coordinates.h"
#include <boost/array.hpp>
#include "smartBlocksBlock.h"

namespace coloring {

//~ using namespace SmartBlocks;

class ColoringMsg;
class ColoringAck;

typedef std::shared_ptr<ColoringMsg> ColoringMsgPtr;
typedef std::shared_ptr<ColoringAck> ColoringAckPtr;

const int cNeighborsQuantity( 8);
const int cColorQuantity( 10);

typedef boost::array<int, cColorQuantity> ColorQuantityArray;

const int cColoringMsgId( 3);
const int cColoringAckId( 4);

//~ struct ColorationInfo {
  //~ int scores[ cColorQuantity];
//~ };

class ColoringMsg : public Message {
  ColorQuantityArray scores_;
  Coordinates origine_;
  Coordinates destination_;
public:
  ColoringMsg( Coordinates, Coordinates, ColorQuantityArray, NeighborDirection::Direction);
  ColoringMsg( const ColoringMsgPtr, NeighborDirection::Direction);
  ~ColoringMsg() {}

  ColorQuantityArray scores() { return scores_; };
  Coordinates origine() { return origine_; }
  Coordinates destination() { return destination_; }

  void print();
};

class ColoringAck : public Message {
  Coordinates origine_;
  Coordinates destination_;
  int color_;
public:
  ColoringAck( Coordinates, Coordinates, int, NeighborDirection::Direction);
  ColoringAck( const ColoringAckPtr, NeighborDirection::Direction);
  ~ColoringAck() {}

  Coordinates origine() { return origine_; };
  Coordinates destination() { return destination_; }
  int color() { return color_; }

  void print();
};
}
#endif // COLORING_H_
