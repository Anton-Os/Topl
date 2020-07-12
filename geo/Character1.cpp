#include "Geo_Constructs.hpp"

void Geo_Character1::fill(Topl_SceneManager* sMan) { // Trying with displacements for now
	Topl_GeoNode* gHead = getNextNode();
	sMan->addGeometry("head", gHead, Eigen::Vector3f(0.0f, 0.4f, 0.0));

	Topl_GeoNode* gBody = getNextNode();
	sMan->addGeometry("body", gBody);

	Topl_GeoNode* gLeftArm = getNextNode();
	sMan->addGeometry("leftArm", gLeftArm, Eigen::Vector3f(-0.5f, 0.5f, 0.0));

	Topl_GeoNode* gRightArm = getNextNode();
	sMan->addGeometry("rightArm", gRightArm, Eigen::Vector3f(0.5f, 0.5f, 0.0));

	Topl_GeoNode* gLeftLeg = getNextNode();
	sMan->addGeometry("leftLeg", gLeftLeg, Eigen::Vector3f(-1.0f, -0.4f, 0.0));

	Topl_GeoNode* gRightLeg = getNextNode();
	sMan->addGeometry("rightLeg", gRightLeg, Eigen::Vector3f(1.0f, -0.4f, 0.0));

    // Possibly code for adding connectors
}

void Geo_Character1::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}