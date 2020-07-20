#include "Geo_Constructs.hpp"

void Geo_Character1::fill(Topl_SceneManager* sMan) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting
	
	Topl_GeoNode* gHead = getNextNode();
	sMan->addGeometry("head", gHead);
	sprite = getSprite(CHAR_Head);
	sMan->addTexture("head", sprite->image);

	Topl_GeoNode* gBody = getNextNode();
	sMan->addGeometry("body", gBody);
	sprite = getSprite(CHAR_Body);
	sMan->addTexture("body", sprite->image);

	Topl_GeoNode* gLeftArm = getNextNode();
	sMan->addGeometry("leftArm", gLeftArm);
	sprite = getSprite(CHAR_LeftArm);
	sMan->addTexture("leftArm", sprite->image);

	Topl_GeoNode* gRightArm = getNextNode();
	sMan->addGeometry("rightArm", gRightArm);
	sprite = getSprite(CHAR_RightArm);
	sMan->addTexture("rightArm", sprite->image);

	Topl_GeoNode* gLeftLeg = getNextNode();
	sMan->addGeometry("leftLeg", gLeftLeg);
	sprite = getSprite(CHAR_LeftLeg);
	sMan->addTexture("leftLeg", sprite->image);

	Topl_GeoNode* gRightLeg = getNextNode();
	sMan->addGeometry("rightLeg", gRightLeg);
	sprite = getSprite(CHAR_LeftLeg);
	sMan->addTexture("rightLeg", sprite->image);

    // Possibly code for adding connectors
}

void Geo_Character1::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}