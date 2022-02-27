#include "trees/Humanoid.hpp"

namespace _Humanoid {
	// TODO: Replace hard offsets, offsets need to vary depending on object constructor!
	const Eigen::Vector2f hardQuartRot = Eigen::Vector2f(-1.0 * TOPL_HALF_PI, 0.0); // Hardcoded quarter rotation
	const Eigen::Vector2f hardHalfRot = Eigen::Vector2f(-1.0 * TOPL_PI, 0.0); // Hardcoded half rotation

	static void createHead(Topl_Scene* scene, Geo_Actor* actor, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		actor->updatePos(offset);
		actor->updateRot(hardQuartRot);
		scene->addGeometry(name, actor);

		if(sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createBody (Topl_Scene* scene, Geo_Actor* actor, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		actor->updatePos(offset);
		actor->updateRot(hardQuartRot);
		scene->addGeometry(name, actor);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createLeftArm(Topl_Scene* scene, Geo_Actor* actor, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		actor->updatePos(offset);
		scene->addGeometry(name, actor);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createRightArm(Topl_Scene* scene, Geo_Actor* actor, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		actor->updatePos(offset);
		actor->updateRot(hardHalfRot);
		scene->addGeometry(name, actor);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createLeftLeg(Topl_Scene* scene, Geo_Actor* actor, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		actor->updatePos(offset);
		scene->addGeometry(name, actor);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}

	static void createRightLeg(Topl_Scene* scene, Geo_Actor* actor, Eigen::Vector3f offset, std::string name, Rasteron_Sprite* sprite) {
		actor->updatePos(offset);
		actor->updateRot(hardHalfRot);
		scene->addGeometry(name, actor);

		if (sprite != nullptr) // Check if a texture is associated
			scene->addTexture(name, sprite->image);
	}
}

/* void Geo_Humanoid::updateActor(HUMANOID_Anatomy target, Eigen::Vector3f pos, Eigen::Vector2f rot){
	return;
} */

void Geo_Humanoid::init(Topl_Scene* scene) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting
	
	Geo_Actor* headActor = getNextActor();
	sprite = getSprite(HUMANOID_Head);
	_Humanoid::createHead(scene, headActor, headOffset, getPrefix() + "head", sprite);

	Geo_Actor* leftArmActor = getNextActor();
	sprite = getSprite(HUMANOID_LeftArm);
	_Humanoid::createLeftArm(scene, leftArmActor, leftArmOffset, getPrefix() + "leftArm", sprite);

	Geo_Actor* rightArmActor = getNextActor();
	sprite = getSprite(HUMANOID_RightArm);
	_Humanoid::createRightArm(scene, rightArmActor, rightArmOffset, getPrefix() + "rightArm", sprite);

	Geo_Actor* bodyActor = getNextActor();
	sprite = getSprite(HUMANOID_Body);
	_Humanoid::createBody(scene, bodyActor, bodyOffset, getPrefix() + "body", sprite);

	Geo_Actor* leftLegActor = getNextActor();
	sprite = getSprite(HUMANOID_LeftLeg);
	_Humanoid::createLeftLeg(scene, leftLegActor, leftLegOffset, getPrefix() + "leftLeg", sprite);

	Geo_Actor* rightLegActor = getNextActor();
	sprite = getSprite(HUMANOID_RightLeg);
	_Humanoid::createRightLeg(scene, rightLegActor, rightLegOffset, getPrefix() + "rightLeg", sprite);

	// All physics relevant functionality goes here

	scene->addPhysics(getPrefix() + "head", &head_phys);
	scene->addPhysics(getPrefix() + "body", &body_phys);
	scene->addPhysics(getPrefix() + "leftArm", &leftArm_phys);
	scene->addPhysics(getPrefix() + "rightArm", &rightArm_phys);
	scene->addPhysics(getPrefix() + "leftLeg", &leftLeg_phys);
	scene->addPhysics(getPrefix() + "rightLeg", &rightLeg_phys);

	// Main links "starfish"
	scene->addLink(&body_head_link, getPrefix() + "body", getPrefix() +  "head");
	scene->addLink(&body_leftArm_link, getPrefix() + "body", getPrefix() + "leftArm");
	scene->addLink(&body_rightArm_link, getPrefix() + "body", getPrefix() + "rightArm");
	scene->addLink(&body_leftLeg_link, getPrefix() + "body", getPrefix() + "leftLeg");
	scene->addLink(&body_rightLeg_link, getPrefix() + "body", getPrefix() + "rightLeg");

	// Stability links "pentagon"
	scene->addLink(&head_leftArm_link, getPrefix() + "head", getPrefix() + "leftArm");
	scene->addLink(&head_rightArm_link, getPrefix() + "head", getPrefix() + "rightArm");
	scene->addLink(&leftArm_leftLeg_link, getPrefix() + "leftArm", getPrefix() + "leftLeg");
	scene->addLink(&rightArm_rightLeg_link, getPrefix() + "rightArm", getPrefix() + "rightLeg");
	scene->addLink(&leftLeg_rightLeg_link, getPrefix() + "leftLeg", getPrefix() + "rightLeg"); 
}