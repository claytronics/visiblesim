/*!
 * \file catoms3DBlock.h
 * \brief catoms Block
 * \date 05/03/2015
 * \author Benoît Piranda
 */

#ifndef CATOMS3DBLOCK_H_
#define CATOMS3DBLOCK_H_

#include <stdexcept>

#include "buildingBlock.h"
#include "catoms3DBlockCode.h"
#include "catoms3DGlBlock.h"
#include "cell3DPosition.h"
//#include "catoms3DCapabilities.h"
#include "utils.h"

using namespace BaseSimulator::utils;

//! \namespace Catoms3D
namespace Catoms3D {

/**
    \brief list of rotations around x,y,z axis applied to the initial catom to obtain orientation of indice orientationCode

    tabOrientationAngles[oc] : rotations done to transform initial catom to oc oriented catom

*/
const float tabOrientationAngles[12][3] = { {0,0,0}, {180.0f,0.0f,-90.0f}, {-90.0f,45.0f,-45.0f},
											{90.0f,45.0f,-135.0f}, {-90.0f,45.0f,135.0f}, {90.0f,45.0f,45.0f},
											{0,0,180.0f}, {180.0f,0,90.0f}, {90.0f,-45.0f,135.0f},
											{-90.0f,-45.0f,45.0f}, {90.0f,-45.0f,-45.0f}, {-90.0f,-45.0f,-135.0f} };

/**
    \brief list of connector positions (x,y,z) in catom local coordinates

    tabConnectorPositions[i] : coordinates of connector #i

*/
const float tabConnectorPositions[12][3] = { {1,0,0}, {0,1,0}, {0.5,0.5,M_SQRT2_2},
											 {-0.5,0.5,M_SQRT2_2},{-0.5,-0.5,M_SQRT2_2},{0.5,-0.5,M_SQRT2_2},
											 {-1,0,0}, {0,-1,0}, {-0.5,-0.5,-M_SQRT2_2},
											 {0.5,-0.5,-M_SQRT2_2},{0.5,0.5,-M_SQRT2_2},{-0.5,0.5,-M_SQRT2_2}};


class Catoms3DBlockCode;

/*! \class Catoms3DBlock
    \brief Special treatement and data for 3D quasi-spherical robot
*/
class Catoms3DBlock : public BaseSimulator::BuildingBlock {
public :
	short orientationCode; //!< number of the connector that is along the x axis.


public:
/**
   \brief Constructor
   \param bId: id of the block
   \param bcd : code block function
*/
	Catoms3DBlock(int bId, BlockCodeBuilder bcb);
	~Catoms3DBlock();

	inline virtual Catoms3DGlBlock* getGlBlock() { return static_cast<Catoms3DGlBlock*>(ptrGlBlock); };
	inline void setGlBlock(Catoms3DGlBlock*ptr) { ptrGlBlock=ptr;};
/**
   \brief Show/Hide a catom in the interface
   \param visible: new state of the catom. */
	void setVisible(bool visible);
/**
   \brief Get the interface from the neighbor position in the grid
   \param pos: position of the cell (if in the grid)
   \return return interface if it exists one connected, NULL otherwise */
	P2PNetworkInterface *getInterface(const Cell3DPosition &pos);
/**
   \brief Get the interface from the interface id
   \param id: interface number
   \return return interface if it exists one connected, NULL otherwise */
	inline P2PNetworkInterface *getInterface(int id) const { return P2PNetworkInterfaces[id]; };
/**
   \brief Get the position of the gridcell in the direction of the given connector
   \param connectorId: id of connector (0..11)
   \param pos: position of the cell (if in the grid)
   \return return true if the cell is in the grid, false otherwise. */
	bool getNeighborPos(short connectorId,Cell3DPosition &pos) const;

	int getDirection(P2PNetworkInterface*);
/**
   \brief Get the orientation code from the transformation matrix of the catom
   \param mat: homogeneous transformation matrix
   \return return orientation code. */
	static short getOrientationFromMatrix(const Matrix &mat);
/**
   \brief Get the transformation matrix of the catom from its position in the grid and its orientation code
   \param pos: position of the cell constaining the catom
   \param code: orientation code (number of the connector aligned with x axis)
   \return return homogeneous transformation matrix. */
    static Matrix getMatrixFromPositionAndOrientation(const Cell3DPosition &pos,short code);
/**
   \brief Set the catom in the grid according to a cell position and an orientation code
   \param pos: position of the cell constaining the catom
   \param code: orientation code (number of the connector aligned with x axis)*/
	void setPositionAndOrientation(const Cell3DPosition &pos,short code);

	// MeldInterpreter
	/**
	 * @copydoc BuildingBlock::addNeighbor
	 */
	virtual void addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target);
	/**
	 * @copydoc BuildingBlock::removeNeighbor
	 */
	virtual void removeNeighbor(P2PNetworkInterface *ni);

};

std::ostream& operator<<(std::ostream &stream, Catoms3DBlock const& bb);

}

#endif /* CATOMS3DBLOCK_H_ */
