#include "Geo_Constructs.hpp"

void Geo_Humanoid::fill(Topl_SceneManager* sMan) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting

	const Eigen::Vector3f hardHeadOffset = Eigen::Vector3f(0.0f, 0.09f, 0.0);
	const Eigen::Vector2f hardHeadRot = Eigen::Vector2f(-1.0 * TOPL_HALF_PI, 0.0);
	const Eigen::Vector3f hardBodyOffset = Eigen::Vector3f(0.0f, -0.1f, 0.0);
	const Eigen::Vector2f hardBodyRot = Eigen::Vector2f(-1.0 * TOPL_HALF_PI, 0.0);
	const Eigen::Vector3f hardArmOffset1 = Eigen::Vector3f(0.09f, 0.1f, 0.0);
	const Eigen::Vector3f hardArmOffset2 = Eigen::Vector3f(-0.09f, 0.1f, 0.0);
	const Eigen::Vector3f hardLegOffset1 = Eigen::Vector3f(0.09f, -0.2f, 0.0);
	const Eigen::Vector3f hardLegOffset2 = Eigen::Vector3f(-0.09f, -0.2f, 0.0);
	
	Geo_Component* gHead = getNextNode();
	gHead->updatePos(hardHeadOffset);
	gHead->updateRot(hardHeadRot);
	sMan->addGeometry("head", gHead);
	sprite = getSprite(HUMANOID_Head);
	sMan->addTexture("head", sprite->image);

	Geo_Component* gBody = getNextNode();
	gBody->updatePos(hardBodyOffset);
	gBody->updateRot(hardBodyRot);
	sMan->addGeometry("body", gBody);
	sprite = getSprite(HUMANOID_Body);
	sMan->addTexture("body", sprite->image);

	Geo_Component* gLeftArm = getNextNode();
	gLeftArm->updatePos(hardArmOffset1);
	sMan->addGeometry("leftArm", gLeftArm);
	sprite = getSprite(HUMANOID_LeftArm);
	sMan->addTexture("leftArm", sprite->image);

	Geo_Component* gRightArm = getNextNode();
	gRightArm->updatePos(hardArmOffset2);
	sMan->addGeometry("rightArm", gRightArm);
	sprite = getSprite(HUMANOID_RightArm);
	sMan->addTexture("rightArm", sprite->image);

	Geo_Component* gLeftLeg = getNextNode();
	gLeftLeg->updatePos(hardLegOffset1);
	sMan->addGeometry("leftLeg", gLeftLeg);
	sprite = getSprite(HUMANOID_LeftLeg);
	sMan->addTexture("leftLeg", sprite->image);

	Geo_Component* gRightLeg = getNextNode();
	gRightLeg->updatePos(hardLegOffset2);
	sMan->addGeometry("rightLeg", gRightLeg);
	sprite = getSprite(HUMANOID_RightLeg);
	sMan->addTexture("rightLeg", sprite->image);


    sMan->addPhysics("head", &physProp_head);
	sMan->addPhysics("body", &physProp_body);

	sMan->addConnector(&connector_bodyToHead, "head", "body");
	
	sMan->addForce("head", Eigen::Vector3f(0.0f, 0.03f, 0.0)); // Connector needs to counter-act this force
}

void Geo_Humanoid::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}