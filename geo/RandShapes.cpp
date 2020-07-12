#include "Geo_Constructs.hpp"

Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos){
    return Eigen::Vector3f(pos[0], pos[1] + 0.1f, pos[2]);
}

void Geo_RandShapes::fillSceneManager(Topl_SceneManager* sMan){
    sMan->addGeometry("box", mGeoNode1);
    sMan->addGeometry("box2", mGeoNode2);
	sMan->addGeometry("sphere1", mGeoNode3);
	// sMan->addGeometry("sphere1", mGeoNode3);
}
