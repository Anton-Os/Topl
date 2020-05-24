#include "Geometry_Constructs.hpp"
#include "Physics.hpp"

Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos){
    return Eigen::Vector3f(pos[0], pos[1] + 0.1f, pos[2]);
}

void Geo_RandShapes::fillSceneGraph(Topl_SceneGraph* sceneGraph){
    sceneGraph->addGeometry("box", mGeoNode1);
}

/*  void createRandShapes(Topl_SceneGraph* sceneGraph){
    Geo_Rect2D sheet(0.8, 0.6);
    Topl_GeoNode geoNode1((Geo_RenderObj*)&sheet);

    sceneGraph->addGeometry("sheet", &geoNode1);

    Physics_MoveAbs upMovement(updatePosAbs);
    upMovement.updatePos(3); // Should move up a few steps
} */