/*
 * @file multiRobotsWorld.h
 *
 *  Created on: 14/07/2016
 *      Author: pthalamy
 */

#ifndef MULTIROBOTSWORLD_H_
#define MULTIROBOTSWORLD_H_

#include <vector>

#include "world.h"
#include "openglViewer.h"
#include "vector3D.h"
#include "multiRobotsBlock.h"
#include "trace.h"

namespace MultiRobots {

static const Vector3D defaultBlockSize{71.0, 71.0, 65.0};

class MultiRobotsWorld : public BaseSimulator::World {
protected:   
    GLuint idTextureWall;

    virtual ~MultiRobotsWorld();
public:
    MultiRobotsWorld(const Cell3DPosition &gridSize, const Vector3D &gridScale,
                      int argc, char *argv[]);

    static void deleteWorld();
    static MultiRobotsWorld* getWorld() {
        assert(world != NULL);
        return((MultiRobotsWorld*)world);
    }
    void printInfo() {
        OUTPUT << "I'm a MultiRobotsWorld" << endl;
    }

    virtual MultiRobotsBlock* getBlockById(int bId) {
        return((MultiRobotsBlock*)World::getBlockById(bId));
    }

    virtual void addBlock(bID blockId, BlockCodeBuilder bcb, const Cell3DPosition &pos, const Color &col,
                          short orientation = 0, bool master = false);

    virtual void linkBlock(const Cell3DPosition &pos);
    virtual void loadTextures(const string &str);

    virtual void glDraw();
    virtual void glDrawId();
    virtual void glDrawIdByMaterial();
    virtual void setSelectedFace(int n);
    virtual void exportConfiguration();	
};

std::ostream& operator<<(std::ostream &stream, MultiRobotsBlock const& bb);

inline void deleteWorld() {
    MultiRobotsWorld::deleteWorld();
}

inline MultiRobotsWorld* getWorld() { return(MultiRobotsWorld::getWorld()); }

} // MultiRobots namespace

#endif /* MULTIROBOTSWORLD_H_ */
