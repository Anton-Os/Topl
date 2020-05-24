#include "Geometry.hpp"
#include "Physics.hpp"

#include "Topl_SceneGraph.hpp"

/* This header defines geometry creation mechanisms */

// void createRandShapes(Topl_SceneGraph* sceneGraph);

Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos);

class Geo_RandShapes {
public:
    Geo_RandShapes(Topl_SceneGraph* sceneGraph){
        mBox1 = new Geo_Rect2D(0.8f, 0.6f);
        mGeoNode1 = new Topl_GeoNode((Geo_RenderObj*)mBox1);
        fillSceneGraph(sceneGraph);
    }
    ~Geo_RandShapes(){
        delete mBox1;
        delete mGeoNode1;
    }
private:
    void fillSceneGraph(Topl_SceneGraph* sceneGraph);
    
    Geo_Rect2D* mBox1;
    Topl_GeoNode* mGeoNode1;
    Physics_MoveAbs mUpMovement = Physics_MoveAbs(&updatePosAbs);
};

void createQuadTree(Topl_SceneGraph* sceneGraph, unsigned divs);
