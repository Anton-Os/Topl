#include "Geometry_Constructs.hpp"
#include "Physics.hpp"

Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos){
    return Eigen::Vector3f(pos[0], pos[1] + 0.1f, pos[2]);
}

void Geo_RandShapes::fillSceneGraph(Topl_SceneGraph* sceneGraph){
    sceneGraph->addGeometry("box", mGeoNode1);
    sceneGraph->addGeometry("box2", mGeoNode2);
	// sceneGraph->addGeometry("sphere1", mGeoNode3);
}

void Geo_RandShapes::updateSceneGraph(Topl_SceneGraph* sceneGraph, Timer_EventRatio event){
    tpl_gEntity_cptr geoNode1 = sceneGraph->getGeoNode("box"); // Get current state, copying over is required
	mGeoNode1 = (Topl_GeoNode*)(geoNode1); // Dont know if this casting is safe!!!

    // Perform all necissary updates here
    mGeoNode1->updatePos(Eigen::Vector3f(-0.12f, -0.33f, 0.0f));
    
    sceneGraph->addGeometry("box", mGeoNode1); // Add the object back into the scene graph 
}

/*  void createRandShapes(Topl_SceneGraph* sceneGraph){
    Geo_Rect2D sheet(0.8, 0.6);
    Topl_GeoNode geoNode1((Geo_RenderObj*)&sheet);

    sceneGraph->addGeometry("sheet", &geoNode1);

    Physics_MoveAbs upMovement(updatePosAbs);
    upMovement.updatePos(3); // Should move up a few steps
} */