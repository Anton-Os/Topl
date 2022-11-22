#include "trees/Humanoid.hpp"

static float getSpriteWidth(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_point[0] * 2;
}

static float getSpriteHeight(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_point[1] * 2;
}

static void resizeToSprite(const Rasteron_Sprite* sprite, Geo_FlatSquare* square) {
	float width = sprite->bounds.topRight_point[0] * 2;
	float height = sprite->bounds.topRight_point[1] * 2;

	if (width != height) { // no scale for matching width and height
		float scaleX = height / width;
		scaleX += (scaleX < 1.0f) ? (1.0f - scaleX) / 2 : -1 * ((scaleX - 1.0f) / 2); // half the scale amount
		square->scale(scaleX, AXIS_X);

		float scaleY = width / height;
		scaleY += (scaleY < 1.0f) ? (1.0f - scaleY) / 2 : -1 * ((scaleY - 1.0f) / 2); // half the scale amount
		square->scale(scaleY, AXIS_Y);
	}
}

// Humanoid operations

void Geo_Humanoid::orient(HUMANOID_Anatomy target, const Vec3f& pos, const Vec3f& angles){
	Geo_Actor* actor;
	switch(target){
	case HUMANOID_Head: actor = head; break;
	case HUMANOID_LeftArm: actor = leftArm; break;
	case HUMANOID_RightArm: actor = rightArm; break;
	case HUMANOID_Torso: actor = torso; break;
	case HUMANOID_LeftLeg: actor = leftLeg; break;
	case HUMANOID_RightLeg: actor = rightLeg; break;
	}

	actor->updatePos(pos);
	actor->updateRot(angles);
}

void Geo_Humanoid::orientAll(std::pair<Vec3f, Vec3f> orientations[HUMANOID_PARTS_COUNT]){
	orient(HUMANOID_Head, orientations[HUMANOID_Head].first, orientations[HUMANOID_Head].second);
	orient(HUMANOID_LeftArm, orientations[HUMANOID_LeftArm].first, orientations[HUMANOID_LeftArm].second);
	orient(HUMANOID_RightArm, orientations[HUMANOID_RightArm].first, orientations[HUMANOID_RightArm].second);
	orient(HUMANOID_Torso, orientations[HUMANOID_Torso].first, orientations[HUMANOID_Torso].second);
	orient(HUMANOID_LeftLeg, orientations[HUMANOID_LeftLeg].first, orientations[HUMANOID_LeftLeg].second);
	orient(HUMANOID_RightLeg, orientations[HUMANOID_RightLeg].first, orientations[HUMANOID_RightLeg].second);

	presetLinks();
}

void Geo_Humanoid::addLinks(Topl_Scene* scene, const std::string& prefix){
	// add star links
	scene->addLink(&torso_head_link, prefix + "torso", prefix + "head");
	scene->addLink(&torso_leftArm_link, prefix + "torso", prefix + "leftArm");
	scene->addLink(&torso_rightArm_link, prefix + "torso", prefix + "rightArm");
	scene->addLink(&torso_leftLeg_link, prefix + "torso", prefix + "leftLeg");
	scene->addLink(&torso_rightLeg_link, prefix + "torso", prefix + "rightLeg");

	// add pentagon links
	scene->addLink(&head_leftArm_link, prefix + "head", prefix + "leftArm");
	scene->addLink(&head_rightArm_link, prefix + "head", prefix + "rightArm");
	scene->addLink(&leftArm_leftLeg_link, prefix + "leftArm", prefix + "leftLeg");
	scene->addLink(&rightArm_rightLeg_link, prefix + "rightArm", prefix + "rightLeg");
	scene->addLink(&leftLeg_rightLeg_link, prefix + "leftLeg", prefix + "rightLeg");
}

void Geo_Humanoid::presetLinks(){
	torso_head_link.preset(torso->getPosition(), head->getPosition());
	torso_leftArm_link.preset(torso->getPosition(), leftArm->getPosition());
	torso_rightArm_link.preset(torso->getPosition(), rightArm->getPosition());
	torso_leftLeg_link.preset(torso->getPosition(), leftLeg->getPosition());
	torso_rightLeg_link.preset(torso->getPosition(), rightLeg->getPosition());

	head_leftArm_link.preset(head->getPosition(), leftArm->getPosition());
	head_rightArm_link.preset(head->getPosition(), rightArm->getPosition());
	leftArm_leftLeg_link.preset(leftArm->getPosition(), leftLeg->getPosition());
	rightArm_rightLeg_link.preset(rightArm->getPosition(), rightLeg->getPosition());
	leftLeg_rightLeg_link.preset(leftLeg->getPosition(), rightLeg->getPosition());
}

// Humanoid 2D operations

Geo_Humanoid2D::~Geo_Humanoid2D() {
	for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++) {
		deleteSprite(_sprites[p]);
		delete(_renderObjs[p]);
	}
}

void Geo_Humanoid2D::configure(Topl_Scene* scene) {
	for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++) {
		_sprites[p] = createSprite(createRefImg(_assetPaths[p].c_str()));
		if (_sprites[p] != nullptr) {
			_renderObjs[p] = new Geo_FlatSquare((getSpriteWidth(_sprites[p]) + getSpriteHeight(_sprites[p])) / 2);
			resizeToSprite(_sprites[p], _renderObjs[p]);
		}
		else _renderObjs[p] = new Geo_FlatSquare(0.5f); // default geometry
	}

	head = getNextActor();
	leftArm = getNextActor();
	rightArm = getNextActor();
	torso = getNextActor();
	leftLeg = getNextActor();
	rightLeg = getNextActor();

	for (unsigned p = 0; p < HUMANOID_PARTS_COUNT; p++) {
		(*bodyActors[p])->setRenderObj(_renderObjs[p]); // set render object for body part
		scene->addGeometry(getPrefix() + bodyPartsStr[p], *bodyActors[p]); // add geometry to scene

		if (_sprites[p] != nullptr)
			scene->addTexture(getPrefix() + bodyPartsStr[p], _sprites[p]->image); // add texture to scene

		scene->addPhysics(getPrefix() + bodyPartsStr[p], bodyPhysics[p]); // add physics to scene
	}

	std::pair<Vec3f, Vec3f> orientations[HUMANOID_PARTS_COUNT] = { // shared orientations
		std::make_pair(Vec3f({ 0.0f, 0.3f, 0.0 }), Vec3f({ 0.0, 0.0f })), // Head
		std::make_pair(Vec3f({ -0.35f, 0.1f, 0.0 }), Vec3f({ 0.0, 0.0 })), // Left Arm
		std::make_pair(Vec3f({ 0.35f, 0.1f, 0.0 }), Vec3f({ 0.0, 0.0 })), // Right Arm
		std::make_pair(Vec3f({ 0.0f, 0.0f, 0.0 }), Vec3f({ 0.0, 0.0 })), // torso
		std::make_pair(Vec3f({ -0.15f, -0.35f, 0.0 }), Vec3f({ -MATH_HALF_PI, 0.0 })), // Left Leg
		std::make_pair(Vec3f({ 0.15f, -0.35f, 0.0 }), Vec3f({ MATH_HALF_PI, 0.0 })) // Right Leg
	};

	orientAll(orientations); // setting orientations
	addLinks(scene, getPrefix()); // adding links between body parts
}

// Humanoid 3D operations

#ifdef TOPL_ENABLE_MODEL

void Geo_Humanoid3D::configure(Topl_Scene* scene) {
	for (std::vector<Geo_Node*>::iterator node = _geoNodes.begin(); node != _geoNodes.end(); node++) {
		std::string nodeName = (*node)->getName(); // assign body parts through name matching

		if (nodeName.find("Head") != std::string::npos || nodeName.find("head") != std::string::npos)
			head = *node; // finding head
		else if (nodeName.find("torso") != std::string::npos || nodeName.find("torso") != std::string::npos)
			torso = *node; // finding torso
		else if (nodeName.find("Arm") != std::string::npos || nodeName.find("arm") != std::string::npos) {
			if (nodeName.find("left") != std::string::npos) leftArm = *node; // finding left arm
			else if (nodeName.find("right") != std::string::npos) rightArm = *node; // finding right arm
		}
		else if (nodeName.find("Leg") != std::string::npos || nodeName.find("leg") != std::string::npos) {
			if (nodeName.find("left") != std::string::npos) leftLeg = *node; // finding left leg
			else if (nodeName.find("right") != std::string::npos) rightLeg = *node; // finding right leg
		}
	}

	// addLinks(scene, getPrefix());
}

#endif