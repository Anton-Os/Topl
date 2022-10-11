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

	if (width != height) { // no stretch for matching width and height
		float stretchX = height / width;
		stretchX += (stretchX < 1.0f) ? (1.0f - stretchX) / 2 : -1 * ((stretchX - 1.0f) / 2); // half the stretch amount
		square->modify(stretchTForm, stretchX, AXIS_X);

		float stretchY = width / height;
		stretchY += (stretchY < 1.0f) ? (1.0f - stretchY) / 2 : -1 * ((stretchY - 1.0f) / 2); // half the stretch amount
		square->modify(stretchTForm, stretchY, AXIS_Y);
	}
}

// Humanoid operations

void Geo_Humanoid::orient(HUMANOID_Anatomy target, const Vec3f& pos, const Vec2f& angles){
	Geo_Actor* actor;
	switch(target){
	case HUMANOID_Head: actor = head; break;
	case HUMANOID_LeftArm: actor = leftArm; break;
	case HUMANOID_RightArm: actor = rightArm; break;
	case HUMANOID_Body: actor = body; break;
	case HUMANOID_LeftLeg: actor = leftLeg; break;
	case HUMANOID_RightLeg: actor = rightLeg; break;
	}

	actor->updatePos(pos);
	actor->updateRot(angles);
}

void Geo_Humanoid::orientAll(std::pair<Vec3f, Vec2f> orientations[HUMANOID_PARTS_COUNT]){
	orient(HUMANOID_Head, orientations[HUMANOID_Head].first, orientations[HUMANOID_Head].second);
	orient(HUMANOID_LeftArm, orientations[HUMANOID_LeftArm].first, orientations[HUMANOID_LeftArm].second);
	orient(HUMANOID_RightArm, orientations[HUMANOID_RightArm].first, orientations[HUMANOID_RightArm].second);
	orient(HUMANOID_Body, orientations[HUMANOID_Body].first, orientations[HUMANOID_Body].second);
	orient(HUMANOID_LeftLeg, orientations[HUMANOID_LeftLeg].first, orientations[HUMANOID_LeftLeg].second);
	orient(HUMANOID_RightLeg, orientations[HUMANOID_RightLeg].first, orientations[HUMANOID_RightLeg].second);

	presetLinks();
}

void Geo_Humanoid::presetLinks(){
	body_head_link.preset(body->getPosition(), head->getPosition());
	body_leftArm_link.preset(body->getPosition(), leftArm->getPosition());
	body_rightArm_link.preset(body->getPosition(), rightArm->getPosition());
	body_leftLeg_link.preset(body->getPosition(), leftLeg->getPosition());
	body_rightLeg_link.preset(body->getPosition(), rightLeg->getPosition());

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
		_renderObjs[p] = new Geo_FlatSquare((getSpriteWidth(_sprites[p]) + getSpriteHeight(_sprites[p])) / 2);
		resizeToSprite(_sprites[p], _renderObjs[p]);
	}

	head = getNextActor();
	head->setRenderObj(_renderObjs[HUMANOID_Head]);
	scene->addGeometry(getPrefix() + "head", head);
	if (_sprites[HUMANOID_Head] != nullptr) scene->addTexture(getPrefix() + "head", _sprites[HUMANOID_Head]->image);

	leftArm = getNextActor();
	leftArm->setRenderObj(_renderObjs[HUMANOID_LeftArm]);
	scene->addGeometry(getPrefix() + "leftArm", leftArm);
	if (_sprites[HUMANOID_LeftArm] != nullptr) scene->addTexture(getPrefix() + "leftArm", _sprites[HUMANOID_LeftArm]->image);

	rightArm = getNextActor();
	rightArm->setRenderObj(_renderObjs[HUMANOID_RightArm]);
	scene->addGeometry(getPrefix() + "rightArm", rightArm);
	if (_sprites[HUMANOID_RightArm] != nullptr) scene->addTexture(getPrefix() + "rightArm", _sprites[HUMANOID_RightArm]->image);

	body = getNextActor();
	body->setRenderObj(_renderObjs[HUMANOID_Body]);
	scene->addGeometry(getPrefix() + "body", body);
	if (_sprites[HUMANOID_Body] != nullptr) scene->addTexture(getPrefix() + "body", _sprites[HUMANOID_Body]->image);

	leftLeg = getNextActor();
	leftLeg->setRenderObj(_renderObjs[HUMANOID_LeftLeg]);
	scene->addGeometry(getPrefix() + "leftLeg", leftLeg);
	if (_sprites[HUMANOID_LeftLeg] != nullptr) scene->addTexture(getPrefix() + "leftLeg", _sprites[HUMANOID_LeftLeg]->image);

	rightLeg = getNextActor();
	rightLeg->setRenderObj(_renderObjs[HUMANOID_RightLeg]);
	scene->addGeometry(getPrefix() + "rightLeg", rightLeg);
	if (_sprites[HUMANOID_RightLeg] != nullptr) scene->addTexture(getPrefix() + "rightLeg", _sprites[HUMANOID_RightLeg]->image);

	// Orientations update

	std::pair<Vec3f, Vec2f> orientations[HUMANOID_PARTS_COUNT] = { // shared orientations
		std::make_pair(Vec3f({ 0.0f, 0.3f, 0.0 }), Vec2f({ 0.0, 0.0f })), // Head
		std::make_pair(Vec3f({ -0.35f, 0.1f, 0.0 }), Vec2f({ 0.0, 0.0 })), // Left Arm
		std::make_pair(Vec3f({ 0.35f, 0.1f, 0.0 }), Vec2f({ 0.0, 0.0 })), // Right Arm
		std::make_pair(Vec3f({ 0.0f, 0.0f, 0.0 }), Vec2f({ 0.0, 0.0 })), // Body
		std::make_pair(Vec3f({ -0.15f, -0.35f, 0.0 }), Vec2f({ -MATH_HALF_PI, 0.0 })), // Left Leg
		std::make_pair(Vec3f({ 0.15f, -0.35f, 0.0 }), Vec2f({ MATH_HALF_PI, 0.0 })) // Right Leg
	};

	orientAll(orientations);

	// All physics functionality

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

// Humanoid 3D operations

void Geo_Humanoid3D::configure(Topl_Scene* scene) {
	// Assignment through node name matching
	for (std::vector<Geo_Node*>::iterator node = _geoNodes.begin(); node != _geoNodes.end(); node++) {
		std::string nodeName = (*node)->getName();

		if (nodeName.find("Head") != std::string::npos || nodeName.find("head") != std::string::npos)
			head = *node;
		else if (nodeName.find("Body") != std::string::npos || nodeName.find("body") != std::string::npos)
			body = *node;
		else if (nodeName.find("Arm") != std::string::npos || nodeName.find("arm") != std::string::npos) {
			// TODO: Improve left and right part logic
			if (nodeName.find("left") != std::string::npos) leftArm = *node;
			else if (nodeName.find("right") != std::string::npos) rightArm = *node;
		}
		else if (nodeName.find("Leg") != std::string::npos || nodeName.find("leg") != std::string::npos) {
			// TODO: Improve left and right part logic
			if (nodeName.find("left") != std::string::npos) leftLeg = *node;
			else if (nodeName.find("right") != std::string::npos) rightLeg = *node;
		}
	}
}