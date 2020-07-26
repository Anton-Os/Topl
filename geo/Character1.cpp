#include "Geo_Constructs.hpp"

void Geo_Character1::fill(Topl_SceneManager* sMan) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting

	const Eigen::Vector3f hardHeadOffset = Eigen::Vector3f(0.0f, 0.3f, 0.0);
	const Eigen::Vector3f hardArmOffset = Eigen::Vector3f(0.25f, 0.1f, 0.0);
	const Eigen::Vector3f hardLegOffset = Eigen::Vector3f(0.25f, -0.2f, 0.0);
	
	Geo_Component* gHead = getNextNode();
	gHead->updatePos(hardHeadOffset);
	sMan->addGeometry("head", gHead);
	sprite = getSprite(CHAR_Head);
	sMan->addTexture("head", sprite->image);

	Geo_Component* gBody = getNextNode();
	sMan->addGeometry("body", gBody);
	sprite = getSprite(CHAR_Body);
	sMan->addTexture("body", sprite->image);

	Geo_Component* gLeftArm = getNextNode();
	sMan->addGeometry("leftArm", gLeftArm);
	sprite = getSprite(CHAR_LeftArm);
	sMan->addTexture("leftArm", sprite->image);

	Geo_Component* gRightArm = getNextNode();
	sMan->addGeometry("rightArm", gRightArm);
	sprite = getSprite(CHAR_RightArm);
	sMan->addTexture("rightArm", sprite->image);

	Geo_Component* gLeftLeg = getNextNode();
	sMan->addGeometry("leftLeg", gLeftLeg);
	sprite = getSprite(CHAR_LeftLeg);
	sMan->addTexture("leftLeg", sprite->image);

	Geo_Component* gRightLeg = getNextNode();
	sMan->addGeometry("rightLeg", gRightLeg);
	sprite = getSprite(CHAR_LeftLeg);
	sMan->addTexture("rightLeg", sprite->image);


    sMan->addPhysics("head", &physProp_head);
	sMan->addPhysics("body", &physProp_body);
	sMan->addConnector(&connector_bodyToHead, "head", "body");
}

void Geo_Character1::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}