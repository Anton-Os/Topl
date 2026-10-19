#include "Topl_Ragdolls.hpp"

static const float stiffnessValues[3] = { 1500.0F, PHYS_DEFAULT_K, PHYS_ROD_K }; // spring constants for loose, normal and rigid joints
static const char* stiffnessNames[3] = { "Loose springs", "Normal springs", "Rigid rods" };
static const float partMasses[PUPPET_PARTS] = { 14.0F, 6.0F, 5.0F, 5.0F, 6.0F, 6.0F }; // body, head, arms, legs
static const Vec3f blockPose[PUPPET_PARTS] = { // standing pose relative to the body center
	Vec3f({ 0.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, 0.62F, 0.0F }),
	Vec3f({ -0.42F, 0.08F, 0.0F }), Vec3f({ 0.42F, 0.08F, 0.0F }),
	Vec3f({ -0.15F, -0.82F, 0.0F }), Vec3f({ 0.15F, -0.82F, 0.0F })
};
static const Vec3f blockDims[PUPPET_PARTS] = { // width, length along the joint, depth
	Vec3f({ 0.5F, 0.75F, 0.3F }), Vec3f({ 0.45F, 0.45F, 0.45F }),
	Vec3f({ 0.16F, 0.7F, 0.16F }), Vec3f({ 0.16F, 0.7F, 0.16F }),
	Vec3f({ 0.2F, 0.85F, 0.2F }), Vec3f({ 0.2F, 0.85F, 0.2F })
};
static const Modern_Float4 puppetColors[RAGDOLL_PUPPETS + 1] = {
	Modern_Float4(0.9F, 0.3F, 0.25F, 1.0F), Modern_Float4(0.25F, 0.6F, 0.95F, 1.0F), Modern_Float4(0.35F, 0.85F, 0.45F, 1.0F),
	Modern_Float4(0.95F, 0.75F, 0.25F, 1.0F), Modern_Float4(0.75F, 0.45F, 0.95F, 1.0F)
};

static float randRange(float lo, float hi){ return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX); }

static Vec3f getJointAxis(const Ragdoll_Body& body, unsigned part, const Vec3f* positions){ // direction a part points along
	Vec3f legs = (positions[PUPPET_LeftLeg] + positions[PUPPET_RightLeg]) * 0.5F;
	switch(part){
		case PUPPET_Body: return runtimeNormalize(positions[PUPPET_Head] - legs);
		case PUPPET_Head: return runtimeNormalize(positions[PUPPET_Head] - positions[PUPPET_Body]);
		default: return runtimeNormalize(positions[PUPPET_Body] - positions[part]);
	}
}

Topl_Ragdolls::~Topl_Ragdolls(){
	for(auto b = _bodies.begin(); b != _bodies.end(); b++){
#ifdef TOPL_ENABLE_TEXTURES
		if(b->isFlat){ delete static_cast<Geo_Puppet2D*>(b->puppet); continue; } // sprite puppets own their images
#endif
		delete b->puppet;
	}
}

// Puppets

void Topl_Ragdolls::placePose(Ragdoll_Body* body, Vec3f origin, float tilt){
	float c = (float)cos(tilt), s = (float)sin(tilt);
	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		Vec3f offset = body->pose[p];
		Vec3f turned = Vec3f({ c * offset.data[0] - s * offset.data[1], s * offset.data[0] + c * offset.data[1], offset.data[2] }); // leans around z
		Geo_Actor* actor = body->puppet->getGeoActor(p);
		actor->setPos(origin + turned);
		Phys_Actor* phys = body->puppet->getPhysActor(p);
		if(phys != nullptr) phys->velocity = Vec3f({ 0.0F, 0.0F, 0.0F });
	}
}

void Topl_Ragdolls::buildBlockPuppet(unsigned index, Vec3f spawn, Modern_Float4 color){
	Geo_Mesh* cube = meshLibrary.getMesh(1);
	Geo_Mesh* orb = meshLibrary.getMesh(0);
	float cubeExtent = getMeshHeight(cube), orbExtent = getMeshHeight(orb);

	Geo_Actor parts[PUPPET_PARTS]; // fresh actors so every part gets its own picking id
	puppetBlocks blocks;
	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		bool isHead = p == PUPPET_Head;
		float extent = (isHead)? orbExtent : cubeExtent;
		parts[p].setMesh((isHead)? orb : cube);
		parts[p].setSize(blockDims[p] * (1.0F / extent));
		blocks[p] = &parts[p];
	}

	Ragdoll_Body body;
	body.puppet = new Geo_Puppet("ragdoll_" + std::to_string(index), blocks);
	body.isBlocks = true;
	body.spawn = spawn;
	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		body.pose[p] = blockPose[p];
		body.radii[p] = blockDims[p].data[2] * 0.5F;
	}
	placePose(&body, spawn, 0.0F); // links take their rest lengths from the standing pose
	body.puppet->configure(&_ragdollScene);
	body.puppet->addPhysics(&_ragdollScene);

	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		const Geo_Actor* actor = body.puppet->getGeoActor(p);
		body.puppet->getPhysActor(p)->mass = partMasses[p];
		Modern_Float4 tint = color;
		if(p == PUPPET_Head) tint = Modern_Float4(0.95F, 0.82F, 0.7F, 1.0F);
		else if(p != PUPPET_Body) for(unsigned c = 0; c < 3; c++) tint[c] *= 0.7F;
		Modern_VertexShader::setActorColor(actor, tint);
		Modern_VertexShader::setActorParams(actor, Modern_Float4(48.0F, 0.15F, 0.0F, (p == PUPPET_Head)? 1.0F : 0.0F));
	}
	_bodies.push_back(body);
}

#ifdef TOPL_ENABLE_TEXTURES
void Topl_Ragdolls::buildSpritePuppet(unsigned index, Vec3f spawn, const std::string& character){
	std::string dir = std::string(IMAGES_DIR) + "characters/" + character;
	bool isWinged = character != "Ghost";
	puppetSpritePaths paths = { // same sprite sheets as the legacy Animatrix demo
		dir + "-Torso.png", dir + "-Head.png",
		dir + ((isWinged)? "-LeftWing.png" : "-LeftArm.png"), dir + ((isWinged)? "-RightWing.png" : "-RightArm.png"),
		dir + "-LeftLeg.png", dir + "-RightLeg.png"
	};

	Ragdoll_Body body;
	Geo_Puppet2D* puppet = new Geo_Puppet2D("sprite_" + std::to_string(index), paths);
	body.puppet = puppet;
	body.isFlat = true;
	body.spawn = spawn;

	// sprite sizes come from the images, so the puppet is scaled to stand about two units tall
	float top = puppet->getGeoActor(PUPPET_Head)->getPos()->data[1];
	float bottom = puppet->getGeoActor(PUPPET_LeftLeg)->getPos()->data[1];
	float spread = top - bottom;
	float factor = (spread > 0.001F)? 1.7F / spread : 1.0F;
	Vec3f bodyPos = *puppet->getGeoActor(PUPPET_Body)->getPos();
	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		Geo_Actor* actor = puppet->getGeoActor(p);
		body.pose[p] = (*actor->getPos() - bodyPos) * factor;
		actor->setSize({ factor, factor, factor });

		float halfHeight = 0.0F;
		const Geo_Mesh* mesh = actor->getMesh();
		for(unsigned v = 0; v < mesh->getVertexCount(); v++){
			float y = (float)fabs(mesh->getVertices()[v].position.data[1]);
			if(y > halfHeight) halfHeight = y;
		}
		body.radii[p] = (halfHeight > 0.0F)? halfHeight * factor * 0.6F : 0.25F;
	}
	placePose(&body, spawn, 0.0F);
	puppet->configure(&_ragdollScene); // adds geometry, sprite textures and the physics links

	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		const Geo_Actor* actor = puppet->getGeoActor(p);
		if(puppet->getPhysActor(p) != nullptr) puppet->getPhysActor(p)->mass = partMasses[p];
		Modern_VertexShader::setActorColor(actor, Modern_Float4(1.0F, 1.0F, 1.0F, 1.0F));
		Modern_VertexShader::setActorParams(actor, Modern_Float4(16.0F, 0.0F, 0.15F, 0.0F));
		Modern_VertexShader::setActorTexture(actor, Modern_Float4(1.0F, 1.0F, 0.0F, 2.0F)); // sprite: texture color and alpha
	}
	_bodies.push_back(body);
}
#endif

Ragdoll_Body* Topl_Ragdolls::findBody(const Geo_Actor* actor, unsigned* part){
	for(auto b = _bodies.begin(); b != _bodies.end(); b++)
		for(unsigned p = 0; p < PUPPET_PARTS; p++)
			if(b->puppet->getGeoActor(p) == actor){ *part = p; return &(*b); }
	return nullptr;
}

void Topl_Ragdolls::drop(){
	for(auto b = _bodies.begin(); b != _bodies.end(); b++){
		Vec3f origin = b->spawn + Vec3f({ randRange(-1.0F, 1.0F), randRange(0.0F, 2.5F), (b->isFlat)? 0.0F : randRange(-1.0F, 1.0F) });
		placePose(&(*b), origin, randRange(-0.6F, 0.6F));
	}
}

void Topl_Ragdolls::setStiffness(RAGDOLL_Stiffness stiffness){
	_stiffness = stiffness;
	for(auto b = _bodies.begin(); b != _bodies.end(); b++)
		for(unsigned l = 0; l < PUPPET_LINKS + 1; l++){
			Phys_Connector* link = b->puppet->getPhysLink(l);
			if(link != nullptr) link->kVal = stiffnessValues[stiffness];
		}
	std::cout << stiffnessNames[stiffness] << std::endl;
}

// Simulation

void Topl_Ragdolls::collide(Ragdoll_Body* body){
	Vec3f positions[PUPPET_PARTS];
	for(unsigned p = 0; p < PUPPET_PARTS; p++) positions[p] = *body->puppet->getGeoActor(p)->getPos();

	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		Geo_Actor* actor = body->puppet->getGeoActor(p);
		Phys_Actor* phys = body->puppet->getPhysActor(p);
		if(phys == nullptr) continue;
		Vec3f pos = *actor->getPos();
		Vec3f& vel = phys->velocity;

		if(pos.data[0] != pos.data[0] || pos.data[1] != pos.data[1] || pos.data[2] != pos.data[2]){ // recover from a blown up simulation
			placePose(body, body->spawn + Vec3f({ 0.0F, 3.0F, 0.0F }), 0.0F);
			return;
		}

		float radius = body->radii[p];
		if(body->isBlocks){ // boxes standing on end reach further down than boxes lying flat
			Vec3f axis = getJointAxis(*body, p, positions);
			float halfLength = blockDims[p].data[1] * 0.5F;
			radius += (halfLength - radius) * (float)fabs(axis.data[1]);
		}

		if(pos.data[1] < radius){ // floor
			pos.data[1] = radius;
			if(vel.data[1] < 0.0F) vel.data[1] *= -0.3F;
			vel.data[0] *= 0.88F; vel.data[2] *= 0.88F;
		}
		for(unsigned a = 0; a < 3; a += 2){ // arena walls
			float limit = (a == 2 && body->isFlat)? RAGDOLL_LANE_Z + 1.0F : RAGDOLL_ARENA;
			if(pos.data[a] > limit){ pos.data[a] = limit; if(vel.data[a] > 0.0F) vel.data[a] *= -0.5F; }
			else if(pos.data[a] < -limit){ pos.data[a] = -limit; if(vel.data[a] < 0.0F) vel.data[a] *= -0.5F; }
		}
		if(body->isFlat){ pos.data[2] = RAGDOLL_LANE_Z; vel.data[2] = 0.0F; } // sprites stay on their lane
		actor->setPos(pos);
	}
}

void Topl_Ragdolls::orient(Ragdoll_Body* body){
	Vec3f positions[PUPPET_PARTS];
	for(unsigned p = 0; p < PUPPET_PARTS; p++) positions[p] = *body->puppet->getGeoActor(p)->getPos();

	for(unsigned p = 0; p < PUPPET_PARTS; p++){
		Vec3f axis = getJointAxis(*body, p, positions);
		Geo_Actor* actor = body->puppet->getGeoActor(p);
		if(body->isBlocks){ // local up follows the joint: roll around z after a pitch around x
			float pitch = (float)asin((axis.data[2] < -1.0F)? -1.0F : (axis.data[2] > 1.0F)? 1.0F : axis.data[2]);
			float roll = (float)atan2(-axis.data[0], axis.data[1]);
			actor->setRot({ roll, pitch, 0.0F });
		} else { // sprites turn around z relative to their resting direction
			Vec3f rest = getJointAxis(*body, p, body->pose);
			float roll = (float)atan2(-axis.data[0], axis.data[1]) - (float)atan2(-rest.data[0], rest.data[1]);
			actor->setRot({ roll, 0.0F, 0.0F });
		}
	}
}

void Topl_Ragdolls::stepPhysics(double frameSecs){
	double realSecs = _physicsTimer.getRelSecs();
	std::vector<std::pair<Vec3f, Vec3f>> saved;
	bool isHeld = isPaused || _isFrozen || realSecs > PHYS_CALC_THRESH; // long frames would launch the springs
	if(isHeld){ // the scene still steps so its own timer stays current, then every part is put back
		for(auto b = _bodies.begin(); b != _bodies.end(); b++)
			for(unsigned p = 0; p < PUPPET_PARTS; p++)
				saved.push_back({ *b->puppet->getGeoActor(p)->getPos(), b->puppet->getPhysActor(p)->velocity });
		_ragdollScene.resolvePhysics();
		unsigned s = 0;
		for(auto b = _bodies.begin(); b != _bodies.end(); b++)
			for(unsigned p = 0; p < PUPPET_PARTS; p++, s++){
				b->puppet->getGeoActor(p)->setPos(saved[s].first);
				b->puppet->getPhysActor(p)->velocity = saved[s].second;
			}
		return;
	}

	float t = (float)elapsed;
	Vec3f grabTarget = fps.position + fps.getForward() * 4.0F;
	unsigned grabPart = 0;
	Ragdoll_Body* grabBody = (_isGrab && getSelected() != nullptr)? findBody(getSelected(), &grabPart) : nullptr;

	for(auto b = _bodies.begin(); b != _bodies.end(); b++)
		for(unsigned p = 0; p < PUPPET_PARTS; p++){
			const Geo_Actor* actor = b->puppet->getGeoActor(p);
			Phys_Actor* phys = b->puppet->getPhysActor(p);
			float mass = (float)phys->mass;
			Vec3f force = Vec3f({ 0.0F, -RAGDOLL_GRAVITY * mass * _gravityScale, 0.0F });
			if(_isWind) force = force + Vec3f({ (0.7F + 0.5F * (float)sin(t * 0.5F + p)) * mass * 6.0F, 0.0F, (float)sin(t * 0.3F) * mass * 2.0F });
			if(grabBody == &(*b) && grabPart == p) // damped spring toward a point in front of the camera
				force = force + (grabTarget - *actor->getPos()) * (mass * 40.0F) - phys->velocity * (mass * 6.0F);
			_ragdollScene.addForce(actor->getName(), force);
		}

	_ragdollScene.resolvePhysics();
	for(auto b = _bodies.begin(); b != _bodies.end(); b++){
		collide(&(*b));
		orient(&(*b));
	}
}

// Demo

void Topl_Ragdolls::init(){
	setEnvironment(1); // sunset
	fps.isFlying = false;
	setStart(Vec3f({ 0.0F, 1.7F, -12.0F }), 0.0F, -0.12F);

	for(unsigned d = 0; d < RAGDOLL_PUPPETS; d++)
		buildBlockPuppet(d, Vec3f({ -6.0F + d * 4.0F, 3.5F + d * 0.8F, -1.0F + (d % 2) * 2.0F }), puppetColors[d]);

	buildBlockPuppet(RAGDOLL_PUPPETS, _anchorPoint - Vec3f({ 0.0F, 2.2F, 0.0F }), puppetColors[RAGDOLL_PUPPETS]); // marionette
	Ragdoll_Body& marionette = _bodies.back();
	const Geo_Actor* head = marionette.puppet->getGeoActor(PUPPET_Head);
	_anchorLink.preset(*head->getPos(), _anchorPoint);
	_anchorLink.kVal = 4000.0;
	_ragdollScene.addAnchor(&_anchorLink, head->getName(), &_anchorPoint);

#ifdef TOPL_ENABLE_TEXTURES
	static const char* characters[RAGDOLL_SPRITES] = { "Angel", "Ghost", "Demon" };
	for(unsigned s = 0; s < RAGDOLL_SPRITES; s++)
		buildSpritePuppet(s, Vec3f({ -5.0F + s * 5.0F, 3.0F + s * 0.6F, RAGDOLL_LANE_Z }), characters[s]);
#endif

	_anchorActor.setName("ragdoll_anchor");
	_anchorActor.setMesh(meshLibrary.getMesh(0));
	_anchorActor.setSize({ 0.3F, 0.3F, 0.3F });
	_anchorActor.setPos(_anchorPoint);
	_propScene.addGeometry(&_anchorActor);
	Modern_VertexShader::setActorColor(&_anchorActor, Modern_Float4(1.0F, 0.85F, 0.4F, 1.0F));
	Modern_VertexShader::setActorParams(&_anchorActor, Modern_Float4(32.0F, 0.0F, 1.5F, 1.0F));

	Modern_VertexShader::setActorColor(&_floorActor, Modern_Float4(0.42F, 0.4F, 0.44F, 1.0F));
	Modern_VertexShader::setActorParams(&_floorActor, Modern_Float4(12.0F, 0.1F, 0.0F, 0.0F));
	Modern_VertexShader::setActorTexture(&_floorActor, Modern_Float4(0.0F, 1.0F, 0.0F, 1.0F));

	buildScene(&_floorScene);
	buildScene(&_ragdollScene);
	buildScene(&_propScene);
	for(auto b = _bodies.begin(); b != _bodies.end(); b++)
		for(unsigned p = 0; p < PUPPET_PARTS; p++) addEditable(b->puppet->getGeoActor(p), &_ragdollScene); // pick parts to grab or kick

	_ragdollScene.resolvePhysics(); // starts the scene timer before the first frame
	_physicsTimer.reset();
}

void Topl_Ragdolls::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: drop(); std::cout << "Drop!" << std::endl; break;
		case KEY_2: // scatter every part
			for(auto b = _bodies.begin(); b != _bodies.end(); b++)
				for(unsigned p = 0; p < PUPPET_PARTS; p++)
					b->puppet->getPhysActor(p)->velocity = Vec3f({ randRange(-9.0F, 9.0F), randRange(4.0F, 14.0F), (b->isFlat)? 0.0F : randRange(-9.0F, 9.0F) });
			std::cout << "Explode!" << std::endl;
			break;
		case KEY_3:
			_gravityScale = (_gravityScale >= 1.0F)? 0.25F : (_gravityScale > 0.0F)? 0.0F : 1.0F;
			std::cout << "Gravity x" << _gravityScale << std::endl;
			break;
		case KEY_4: _isWind = !_isWind; std::cout << "Wind " << ((_isWind)? "on" : "off") << std::endl; break;
		case KEY_5: setStiffness((RAGDOLL_Stiffness)((_stiffness + 1) % 3)); break;
		case KEY_6:
			_isGrab = !_isGrab;
			std::cout << ((_isGrab)? "Grabbing the selected part, click a part to select it" : "Released") << std::endl;
			break;
		case KEY_7: { // kick the selected part, or everyone when nothing is selected
			unsigned part = 0;
			Ragdoll_Body* target = (getSelected() != nullptr)? findBody(getSelected(), &part) : nullptr;
			Vec3f kick = fps.getForward() * 14.0F + Vec3f({ 0.0F, 4.0F, 0.0F });
			for(auto b = _bodies.begin(); b != _bodies.end(); b++)
				for(unsigned p = 0; p < PUPPET_PARTS; p++)
					if(target == nullptr || (target == &(*b) && part == p)){
						Phys_Actor* phys = b->puppet->getPhysActor(p);
						phys->velocity = phys->velocity + ((target == nullptr)? kick * 0.5F : kick);
					}
			break;
		}
		case KEY_8: _isSwinging = !_isSwinging; std::cout << "Marionette " << ((_isSwinging)? "swinging" : "still") << std::endl; break;
		case KEY_9:
			_isLane = !_isLane;
			for(auto b = _bodies.begin(); b != _bodies.end(); b++) if(b->isFlat) b->puppet->toggleShow(_isLane);
			break;
		case KEY_0: _isFrozen = !_isFrozen; std::cout << "Physics " << ((_isFrozen)? "frozen" : "running") << std::endl; break;
		default: break;
	}
}

void Topl_Ragdolls::loop(double frameSecs){
	float t = (float)elapsed;
	if(_isSwinging) _anchorPoint = Vec3f({ (float)sin(t * 0.9F) * 3.0F, 6.0F + (float)sin(t * 1.7F) * 0.5F, 4.0F + (float)cos(t * 0.6F) * 1.5F });
	_anchorActor.setPos(_anchorPoint);

	stepPhysics(frameSecs);

	renderSky();
	renderScene(&_floorScene, MODERN_Lighting, (shaderMode == 1)? 0 : shaderMode);
	renderScene(&_propScene, MODERN_Lighting, shaderMode);
	renderScene(&_ragdollScene, MODERN_Lighting, shaderMode);
}

void Topl_Ragdolls::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Ragdolls" << std::endl
		<< "  1 drop   2 explode   3 gravity   4 wind   5 spring stiffness   6 grab the selected part   7 kick" << std::endl
		<< "  8 marionette swing   9 show or hide the 2D lane   0 freeze physics" << std::endl
		<< "  Click a body part to select it, then 6 holds it in front of the camera and 7 kicks it" << std::endl << std::endl;
}
