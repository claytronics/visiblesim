/** 
 * @file configExporter.h
 * Header for Configuration Exporter module
 */


#ifndef CONFIGEXPORTER_H__
#define CONFIGEXPORTER_H__

#define TIXML_USE_STL	1
#include "TinyXML/tinyxml.h"

#include "world.h"
#include "buildingBlock.h"
#include "openglViewer.h"
#include "camera.h"

using namespace std;

namespace BaseSimulator {

/************************************************************
 *   Abstract Configuration Exporter
 ************************************************************/    

/** 
 * @brief Abstract Configuration Exporter
 *
 * Saves all the information in the world at time of export into an xml file
 *  with name config_hh_mm_ss.xml. The following elements are exported: 
 *  1. Properties of the world (dimensions, scale) 
 *  2. The current state of the camera and lightsource
 *  3. The list of blocks and their current attributes. 
 *    (Common ones + type specific ones exported by the virtual function exportAdditionalAttribute)  
 */
class ConfigExporter {      
protected:
    World *world;          //!< pointer to the world to export
    TiXmlDocument *config; //!< the TiXML Document used for export 
    string configName;     //!< the name of the output configuration file
    TiXmlElement *worldElt; //!< a pointer to the world XML element of the document
    TiXmlElement *blockListElt; //!< a pointer to the blockList XML element of the document
public:
    /**
     * @brief Constructor for the abstract configuration exporter
     *  Creates the output document, filename and header
     */
    ConfigExporter(World *world);
    /**
     * @brief Destructor for the abstract configuration exporter
     *  Deletes the TiXMLDocument used for exporting
     */
    virtual ~ConfigExporter();

    /**
     * @brief Main function of the configuration exporter, calls all export subfunctions sequentially.
     */
    void exportConfiguration();
    /**
     * @brief Exports the camera and lightSource (Current position and orientation) to the configuration file.
     */
    void exportCameraAndLightSource();
    /**
     * @brief Exports the world and window attributes to the configuration file
     */
    void exportWorld();
    /**
     * @brief Initializes the blockList XML element and calls exportBlock on each block for export.
     *  The default color of the blockList will be the one of the user selected block.
     */
    void exportBlockList();
    /**
     * @brief Exports all the generic attributes of a BuildingBlock
     * @param bb : Pointer to the block to export
     *  If exporting a block family specific attribute is needed, the exportAdditionalAttribute can be used.
     */
    void exportBlock(BuildingBlock *bb);

    /**
     * @brief Exports additional non-generic attributes from block bb.
     * @param bbElt : the TiXmlElement for the current block
     * @param bb : the current block
     */
    virtual void exportAdditionalAttribute(TiXmlElement *bbElt, BuildingBlock *bb) {};
};


/************************************************************
 *   Subclasses
 ************************************************************/    

/** 
 * @brief BlinkyBlocks Configuration Exporter
 */
class BlinkyBlocksConfigExporter : public ConfigExporter {

public:
    /** 
     * @brief BlinkyBlocks Configuration Exporter constructor
     */
    BlinkyBlocksConfigExporter(World *_world) : ConfigExporter(_world) {};
    /** 
     * @brief BlinkyBlocks Configuration Exporter destructor
     */
    virtual ~BlinkyBlocksConfigExporter() { };
};

/** 
 * @brief Catoms3D Configuration Exporter
 */
class Catoms3DConfigExporter : public ConfigExporter {
public:
    /** 
     * @brief Catoms3D Configuration Exporter constructor
     */
    Catoms3DConfigExporter(World *_world) : ConfigExporter(_world) {};
    /** 
     * @brief Catoms3D Configuration Exporter destructor
     */
    virtual ~Catoms3DConfigExporter() { };

    /** 
     * @copydoc ConfigExporter::exportAdditionalAttribute
     *  Exports the rotation attribute of a Catoms3DBlock
     */
    virtual void exportAdditionalAttribute(TiXmlElement *bbElt, BuildingBlock *bb);
};

/** 
 * @brief RobotBlocks Configuration Exporter
 */
class RobotBlocksConfigExporter : public ConfigExporter {
public:
    /** 
     * @brief RobotBlocks Configuration Exporter constructor
     */
    RobotBlocksConfigExporter(World *_world) : ConfigExporter(_world) {};
    /** 
     * @brief RobotBlocks Configuration Exporter destructor
     */
    virtual ~RobotBlocksConfigExporter() { };
};

/** 
 * @brief Catoms2D Configuration Exporter
 */
class Catoms2DConfigExporter : public ConfigExporter {
public:
    /** 
     * @brief Catoms2D Configuration Exporter constructor
     */
    Catoms2DConfigExporter(World *_world) : ConfigExporter(_world) {};
    /** 
     * @brief Catoms2D Configuration Exporter constructor
     */
    virtual ~Catoms2DConfigExporter() { };

    /** 
     * @copydoc ConfigExporter::exportAdditionalAttribute
     *  Exports the rotation attribute of a Catoms3DBlock
     */
    virtual void exportAdditionalAttribute(TiXmlElement *bbElt, BuildingBlock *bb);
};

/** 
 * @brief SmartBlocks Configuration Exporter
 */
class SmartBlocksConfigExporter : public ConfigExporter {
public:
    /** 
     * @brief SmartBlocks Configuration Exporter constructor
     */
    SmartBlocksConfigExporter(World *_world) : ConfigExporter(_world) {};
    /** 
     * @brief SmartBlocks Configuration Exporter constructor
     */
    virtual ~SmartBlocksConfigExporter() { };
};

/** 
 * @brief MultiRobots Configuration Exporter
 */
class MultiRobotsConfigExporter : public ConfigExporter {
public:
    /** 
     * @brief MultiRobots Configuration Exporter constructor
     */
    MultiRobotsConfigExporter(World *_world) : ConfigExporter(_world) {};
    /** 
     * @brief MultiRobots Configuration Exporter constructor
     */
    virtual ~MultiRobotsConfigExporter() { };
};

} // BASESIMULATOR_NAMESPACE

#endif // CONFIGEXPORTER_H__
