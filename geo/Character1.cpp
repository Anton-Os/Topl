#include "Geo_Constructs.hpp"

void Geo_Character1::fill(Topl_SceneManager* sMan) {
	Topl_GeoNode* gHead = getNextNode();
	addGeometry("head", gHead); // TODO: Fix code above to get this working
    // Possibly code for adding connectors
}

void Geo_Character1::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}