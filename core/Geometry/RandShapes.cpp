#include "Geometry_Constructs.hpp"
#include "Physics.hpp"

void createRandShapes(Topl_SceneGraph* sceneGraph){
    Geo_Rect2D sheet(0.8, 0.6);
    Topl_GeoNode geoNode1((Geo_RenderObj*)&sheet);

    sceneGraph->addGeometry("sheet", geoNode1);
}