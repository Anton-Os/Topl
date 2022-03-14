#include "trees/Humanoid.hpp"

void Geo_Humanoid::orient(HUMANOID_Anatomy target, const Eigen::Vector3f& pos, const Eigen::Vector2f& angles){
	Geo_Actor* actor;
	switch(target){
	case HUMANOID_Head: actor = head; break;
	case HUMANOID_LeftArm: actor = leftArm; break;
	case HUMANOID_RightArm: actor = rightArm; break;
	case HUMANOID_Body: actor = body; break;
	case HUMANOID_LeftLeg: actor = leftArm; break;
	case HUMANOID_RightLeg: actor = rightArm; break;
	}

	actor->setPos(pos);
	actor->setRot(angles);
}

void Geo_Humanoid::orientAll(std::pair<Eigen::Vector3f, Eigen::Vector2f> orientations[HUMANOID_PARTS_COUNT]){
	orient(HUMANOID_Head, orientations[HUMANOID_Head].first, orientations[HUMANOID_Head].second);
	orient(HUMANOID_LeftArm, orientations[HUMANOID_LeftArm].first, orientations[HUMANOID_LeftArm].second);
	orient(HUMANOID_RightArm, orientations[HUMANOID_RightArm].first, orientations[HUMANOID_RightArm].second);
	orient(HUMANOID_Body, orientations[HUMANOID_Body].first, orientations[HUMANOID_Body].second);
	orient(HUMANOID_LeftLeg, orientations[HUMANOID_LeftLeg].first, orientations[HUMANOID_LeftLeg].second);
	orient(HUMANOID_RightLeg, orientations[HUMANOID_RightLeg].first, orientations[HUMANOID_RightLeg].second);
}

void Geo_Humanoid2D::init(Topl_Scene* scene) { // Trying with displacements for now
	Rasteron_Sprite* sprite; // Container for all the sprites we are getting
	
	head = getNextActor();
	sprite = getSprite(HUMANOID_Head);
	scene->addGeometry(getPrefix() + "head", head);
	if (sprite != nullptr) scene->addTexture(getPrefix() + "head", sprite->image);

	leftArm = getNextActor();
	sprite = getSprite(HUMANOID_LeftArm);
	scene->addGeometry(getPrefix() + "leftArm", leftArm);
	if (sprite != nullptr) scene->addTexture(getPrefix() + "leftArm", sprite->image);

	rightArm = getNextActor();
	sprite = getSprite(HUMANOID_RightArm);
	scene->addGeometry(getPrefix() + "rightArm", rightArm);
	if (sprite != nullptr) scene->addTexture(getPrefix() + "rightArm", sprite->image);

	body = getNextActor();
	sprite = getSprite(HUMANOID_Body);
	scene->addGeometry(getPrefix() + "body", body);
	if (sprite != nullptr) scene->addTexture(getPrefix() + "body", sprite->image);

	leftLeg = getNextActor();
	sprite = getSprite(HUMANOID_LeftLeg);
	scene->addGeometry(getPrefix() + "leftLeg", leftLeg);
	if (sprite != nullptr) scene->addTexture(getPrefix() + "leftLeg", sprite->image);

	rightLeg = getNextActor();
	sprite = getSprite(HUMANOID_RightLeg);
	scene->addGeometry(getPrefix() + "rightLeg", rightLeg);
	if (sprite != nullptr) scene->addTexture(getPrefix() + "rightLeg", sprite->image);

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