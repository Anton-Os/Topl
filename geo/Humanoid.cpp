#include "Geo_Constructs.hpp"

namespace _Humanoid {
	// TODO: Replace hard offsets, offsets need to vary depending on object constructor!
	const Eigen::Vector2f hardQuartRot = Eigen::Vector2f(-1.0 * TOPL_HALF_PI, 0.0); // Hardcoded quarter rotation
	const Eigen::Vector2f hardHalfRot = Eigen::Vector2f(-1.0 * TOPL_PI, 0.0); // Hardcoded half rotation

	static void createHead(Geo_Component* geoc, std::string name, Rasteron_Sprite* sprite, Topl_SceneManager* sMan) {
		const Eigen::Vector3f hardHeadOffset = Eigen::Vector3f(0.0f, 0.11f, 0.0);

		geoc->updatePos(hardHeadOffset);
		geoc->updateRot(hardQuartRot);
		sMan->addGeometry(name, geoc);

		if(sprite != nullptr) // Check if a texture is associated
			sMan->addTexture(name, sprite->image);
	}

	static void createBody (Geo_Component* geoc, std::string name, Rasteron_Sprite* sprite, Topl_SceneManager* sMan) {
		const Eigen::Vector3f hardBodyOffset = Eigen::Vector3f(0.0f, -0.1f, 0.0);

		geoc->updatePos(hardBodyOffset);
		geoc->updateRot(hardQuartRot);
		sMan->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			sMan->addTexture(name, sprite->image);
	}

	static void createLeftArm(Geo_Component* geoc, std::string name, Rasteron_Sprite* sprite, Topl_SceneManager* sMan) {
		const Eigen::Vector3f hardArmOffset = Eigen::Vector3f(0.12f, -0.14f, 0.0);

		geoc->updatePos(hardArmOffset);
		sMan->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			sMan->addTexture(name, sprite->image);
	}

	static void createRightArm(Geo_Component* geoc, std::string name, Rasteron_Sprite* sprite, Topl_SceneManager* sMan) {
		const Eigen::Vector3f hardArmOffset = Eigen::Vector3f(-0.12f, -0.14f, 0.0);

		geoc->updatePos(hardArmOffset);
		geoc->updateRot(hardHalfRot);
		sMan->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			sMan->addTexture(name, sprite->image);
	}

	static void createLeftLeg(Geo_Component* geoc, std::string name, Rasteron_Sprite* sprite, Topl_SceneManager* sMan) {
		const Eigen::Vector3f hardLegOffset = Eigen::Vector3f(0.06f, -0.35f, 0.0);

		geoc->updatePos(hardLegOffset);
		sMan->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			sMan->addTexture(name, sprite->image);
	}

	static void createRightLeg(Geo_Component* geoc, std::string name, Rasteron_Sprite* sprite, Topl_SceneManager* sMan) {
		const Eigen::Vector3f hardLegOffset = Eigen::Vector3f(-0.06f, -0.35f, 0.0);

		geoc->updatePos(hardLegOffset);
		geoc->updateRot(hardHalfRot);
		sMan->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			sMan->addTexture(name, sprite->image);
	}
}

void Geo_Humanoid::fill(Topl_SceneManager* sMan) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting
	const Eigen::Vector3f hardLegOffset1 = Eigen::Vector3f(0.06f, -0.35f, 0.0);
	const Eigen::Vector3f hardLegOffset2 = Eigen::Vector3f(-0.06f, -0.35f, 0.0);
	
	Geo_Component* geocHead = getNextGeo();
	sprite = getSprite(HUMANOID_Head);
	_Humanoid::createHead(geocHead, getPrefix() + "head", sprite, sMan);

	Geo_Component* geocLeftArm = getNextGeo();
	sprite = getSprite(HUMANOID_LeftArm);
	_Humanoid::createLeftArm(geocLeftArm, getPrefix() + "leftArm", sprite, sMan);

	Geo_Component* geocRightArm = getNextGeo();
	sprite = getSprite(HUMANOID_RightArm);
	_Humanoid::createRightArm(geocRightArm, getPrefix() + "rightArm", sprite, sMan);

	Geo_Component* geocBody = getNextGeo();
	sprite = getSprite(HUMANOID_Body);
	_Humanoid::createBody(geocBody, getPrefix() + "body", sprite, sMan);

	Geo_Component* geocLeftLeg = getNextGeo();
	sprite = getSprite(HUMANOID_LeftLeg);
	_Humanoid::createLeftLeg(geocLeftLeg, getPrefix() + "leftLeg", sprite, sMan);

	Geo_Component* geocRightLeg = getNextGeo();
	sprite = getSprite(HUMANOID_RightLeg);
	_Humanoid::createRightLeg(geocRightLeg, getPrefix() + "rightLeg", sprite, sMan);

	// All physics relevant functionality goes here

	sMan->addPhysics(getPrefix() + "head", &head_phys);
	sMan->addPhysics(getPrefix() + "body", &body_phys);
	sMan->addPhysics(getPrefix() + "leftArm", &leftArm_phys);
	sMan->addPhysics(getPrefix() + "rightArm", &rightArm_phys);
	sMan->addPhysics(getPrefix() + "leftLeg", &leftLeg_phys);
	sMan->addPhysics(getPrefix() + "rightLeg", &rightLeg_phys);

	// Main links "starfish"
	sMan->addConnector(&body_head_link, getPrefix() + "body", getPrefix() +  "head");
	sMan->addConnector(&body_leftArm_link, getPrefix() + "body", getPrefix() + "leftArm");
	sMan->addConnector(&body_rightArm_link, getPrefix() + "body", getPrefix() + "rightArm");
	sMan->addConnector(&body_leftLeg_link, getPrefix() + "body", getPrefix() + "leftLeg");
	sMan->addConnector(&body_rightLeg_link, getPrefix() + "body", getPrefix() + "rightLeg");

	/* // Stability links "pentagon"
	sMan->addConnector(&head_leftArm_link, getPrefix() + "head", getPrefix() + "leftArm");
	sMan->addConnector(&head_rightArm_link, getPrefix() + "head", getPrefix() + "rightArm");
	sMan->addConnector(&leftArm_leftLeg_link, getPrefix() + "leftArm", getPrefix() + "leftLeg");
	sMan->addConnector(&rightArm_rightLeg_link, getPrefix() + "rightArm", getPrefix() + "rightLeg");
	sMan->addConnector(&leftLeg_rightLeg_link, getPrefix() + "leftLeg", getPrefix() + "rightLeg"); */
}

void Geo_Humanoid::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
} 

void Geo_Humanoid::move(Topl_SceneManager* sMan, Eigen::Vector3f vec){
	sMan->addForce(getPrefix() + "head", vec);
	sMan->addForce(getPrefix() + "body", vec);
	sMan->addForce(getPrefix() + "leftArm", vec);
	sMan->addForce(getPrefix() + "rightArm", vec);
	sMan->addForce(getPrefix() + "leftLeg", vec);
	sMan->addForce(getPrefix() + "rightLeg", vec);
}