#include "trees/Humanoid.hpp"

namespace _Humanoid {
	// TODO: Replace hard offsets, offsets need to vary depending on object constructor!
	const Eigen::Vector2f hardQuartRot = Eigen::Vector2f(-1.0 * TOPL_HALF_PI, 0.0); // Hardcoded quarter rotation
	const Eigen::Vector2f hardHalfRot = Eigen::Vector2f(-1.0 * TOPL_PI, 0.0); // Hardcoded half rotation

	static void createHead(Topl_Scene* scene, Geo_Actor* geoc, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		geoc->updatePos(offset);
		geoc->updateRot(hardQuartRot);
		scene->addGeometry(name, geoc);

		if(sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createBody (Topl_Scene* scene, Geo_Actor* geoc, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		geoc->updatePos(offset);
		geoc->updateRot(hardQuartRot);
		scene->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createLeftArm(Topl_Scene* scene, Geo_Actor* geoc, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		geoc->updatePos(offset);
		scene->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createRightArm(Topl_Scene* scene, Geo_Actor* geoc, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		geoc->updatePos(offset);
		geoc->updateRot(hardHalfRot);
		scene->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createLeftLeg(Topl_Scene* scene, Geo_Actor* geoc, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		geoc->updatePos(offset);
		scene->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createRightLeg(Topl_Scene* scene, Geo_Actor* geoc, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		geoc->updatePos(offset);
		geoc->updateRot(hardHalfRot);
		scene->addGeometry(name, geoc);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}
}

void Geo_Humanoid::fill(Topl_Scene* scene) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting
	
	Geo_Actor* geocHead = getNextGeo();
	sprite = getSprite(HUMANOID_Head);
	_Humanoid::createHead(scene, geocHead, headOffset, getPrefix() + "head", sprite);

	Geo_Actor* geocLeftArm = getNextGeo();
	sprite = getSprite(HUMANOID_LeftArm);
	_Humanoid::createLeftArm(scene, geocLeftArm, leftArmOffset, getPrefix() + "leftArm", sprite);

	Geo_Actor* geocRightArm = getNextGeo();
	sprite = getSprite(HUMANOID_RightArm);
	_Humanoid::createRightArm(scene, geocRightArm, rightArmOffset, getPrefix() + "rightArm", sprite);

	Geo_Actor* geocBody = getNextGeo();
	sprite = getSprite(HUMANOID_Body);
	_Humanoid::createBody(scene, geocBody, bodyOffset, getPrefix() + "body", sprite);

	Geo_Actor* geocLeftLeg = getNextGeo();
	sprite = getSprite(HUMANOID_LeftLeg);
	_Humanoid::createLeftLeg(scene, geocLeftLeg, leftLegOffset, getPrefix() + "leftLeg", sprite);

	Geo_Actor* geocRightLeg = getNextGeo();
	sprite = getSprite(HUMANOID_RightLeg);
	_Humanoid::createRightLeg(scene, geocRightLeg, rightLegOffset, getPrefix() + "rightLeg", sprite);

	// All physics relevant functionality goes here

	scene->addPhysics(getPrefix() + "head", &head_phys);
	scene->addPhysics(getPrefix() + "body", &body_phys);
	scene->addPhysics(getPrefix() + "leftArm", &leftArm_phys);
	scene->addPhysics(getPrefix() + "rightArm", &rightArm_phys);
	scene->addPhysics(getPrefix() + "leftLeg", &leftLeg_phys);
	scene->addPhysics(getPrefix() + "rightLeg", &rightLeg_phys);

	// Main links "starfish"
	scene->addConnector(&body_head_link, getPrefix() + "body", getPrefix() +  "head");
	scene->addConnector(&body_leftArm_link, getPrefix() + "body", getPrefix() + "leftArm");
	scene->addConnector(&body_rightArm_link, getPrefix() + "body", getPrefix() + "rightArm");
	scene->addConnector(&body_leftLeg_link, getPrefix() + "body", getPrefix() + "leftLeg");
	scene->addConnector(&body_rightLeg_link, getPrefix() + "body", getPrefix() + "rightLeg");

	// Stability links "pentagon"
	scene->addConnector(&head_leftArm_link, getPrefix() + "head", getPrefix() + "leftArm");
	scene->addConnector(&head_rightArm_link, getPrefix() + "head", getPrefix() + "rightArm");
	scene->addConnector(&leftArm_leftLeg_link, getPrefix() + "leftArm", getPrefix() + "leftLeg");
	scene->addConnector(&rightArm_rightLeg_link, getPrefix() + "rightArm", getPrefix() + "rightLeg");
	scene->addConnector(&leftLeg_rightLeg_link, getPrefix() + "leftLeg", getPrefix() + "rightLeg"); 
}

void Geo_Humanoid::updateScene(Topl_Scene* scene) {
	_updateCount++;
    return;
}