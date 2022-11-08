#include "Topl_Scene.hpp"

// recomputes connector attributes
static void calcConnectorAttrib(Phys_Connector* connector, const Vec3f& pos1, const Vec3f& pos2){
	Vec3f linkDiff = Vec3f(pos1) - Vec3f(pos2);
	connector->length = linkDiff.len();
	connector->centerPoint = (Vec3f(pos1) + Vec3f(pos2)) * 0.5f;
	connector->angleVecn1 = Vec3f(pos1) - connector->centerPoint;
	connector->angleVecn1.normalize();
	connector->angleVecn2 = Vec3f(pos2) - connector->centerPoint;
	connector->angleVecn2.normalize();
}

// Scene Dynamics

void Topl_Scene::addPhysics(const std::string& name, Phys_Actor* physActor) {
	// Find matching geometry component
	for(std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if((*actor)->getName() == name){
			_actorPhys_map.insert({ *actor, physActor });
			return;
		}
		
	return logMessage(MESSAGE_Exclaim, "Could not locate geometry actor for object: " + name);
}

void Topl_Scene::addForce(const std::string& name, const Vec3f& forceVec) {
	// Find matching geometry component
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			if (_actorPhys_map.find(*actor) == _actorPhys_map.end()) 
				return logMessage(MESSAGE_Exclaim, "Could not locate physics actor for object: " + name); 

			Phys_Actor* physActor = _actorPhys_map.find(*actor)->second;
			vec3f_cptr_t targetPos = (*actor)->getPos();

			if(!physActor->addForce(forceVec)) return logMessage(MESSAGE_Exclaim, "Forces excess!");
		}
}

void Topl_Scene::addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2) {

	const Geo_Actor* link1 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name1 == (*actor)->getName()) link1 = *actor;
	if (link1 == nullptr) return logMessage(MESSAGE_Exclaim, "Could not locate linked actor for object: " + name1);

	const Geo_Actor* link2 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name2 == (*actor)->getName()) link2 = *actor;
	if (link2 == nullptr) return logMessage(MESSAGE_Exclaim, "Could not locate linked actor for object: " + name2);

	if(!connector->getIsPreset()) 
		connector->preset(link1->getPosition(), link2->getPosition()); // presets link data to defaults
		// connector->preset(*link1->getPos(), *link2->getPos()); // presets link data to defaults

	_linkedItems.push_back({connector, std::make_pair(link1, link2)}); // add new link
}

void Topl_Scene::addAnchor(Phys_Connector* connector, const std::string& name, const Vec3f* pos) {

	const Geo_Actor* targetActor = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) targetActor = *actor;
	if (targetActor == nullptr) return logMessage(MESSAGE_Exclaim, "Could not locate linked actor for object: " + name);

	if(!connector->getIsPreset()){
		connector->kVal = PHYS_ROD_K; // TODO: Change to rod type instead of modifying k value
		connector->preset(targetActor->getPosition(), *pos);
	}

	_anchoredItems.push_back({connector, std::make_pair(targetActor, pos)}); // add new anchor
}


void Topl_Scene::remConnector(const std::string& targetName){
	for(std::vector<LinkedItems>::iterator link = _linkedItems.begin(); link != _linkedItems.end(); link++)
		if(link->linkedItems.first->getName() == targetName || link->linkedItems.second->getName() == targetName)
			_linkedItems.erase(link);

	for(std::vector<AnchoredItems>::iterator anchor = _anchoredItems.begin(); anchor != _anchoredItems.end(); anchor++)
		if(anchor->anchoredItems.first->getName() == targetName)
			_anchoredItems.erase(anchor);
}


void Topl_Scene::resolvePhysics() {
	double elapseSecs = _ticker.getRelSecs();

	// Resolve Link Forces
	for(std::vector<LinkedItems>::iterator link = _linkedItems.begin(); link != _linkedItems.end(); link++){
		Phys_Connector* connector = link->connector;
		const Geo_Actor* linkItem1 = link->linkedItems.first;
		const Geo_Actor* linkItem2 = link->linkedItems.second;

		calcConnectorAttrib(connector, linkItem1->getPosition(), linkItem2->getPosition());

		// Forces acting by length displacement
		if(abs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // No forces unless threshhold exceeded!
			// determines direction of force for first geometry link
			Vec3f force1 = (connector->length < connector->restLength)
				? linkItem1->getPosition() - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - linkItem1->getPosition(); // length is longer, pull inward
			force1.normalize(); // normalizing for directional unit vector

			// determines direction of force for second geometry link
			Vec3f force2 = (connector->length < connector->restLength)
				? linkItem2->getPosition() - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - linkItem2->getPosition(); // length is longer, pull inward
			force2.normalize(); // normalizing for directional unit vector

			double lengthDiff = abs(connector->length - connector->restLength); // get how much displacement occured
			const Vec3f forceFinal1 = force1 * ((lengthDiff * connector->kVal) * 0.5f);
			const Vec3f forceFinal2 = force2 * ((lengthDiff * connector->kVal) * 0.5f);

			addForce(linkItem1->getName(), forceFinal1); // add force to first actor
			addForce(linkItem2->getName(), forceFinal2); // add force to second actor
		}

		// Forces acting by angular displacement // TODO: Add a Threshold value!!!
		if (connector->restAngleVecn1 != connector->angleVecn1 && connector->restAngleVecn2 != connector->angleVecn2) {
			const Vec3f forceAngle1 = connector->restAngleVecn1 - connector->angleVecn1;
			const Vec3f forceAngle2 = connector->restAngleVecn2 - connector->angleVecn2;

			addForce(linkItem1->getName(), forceAngle1 * CONNECTOR_ANGLE_SCALE);
			addForce(linkItem2->getName(), forceAngle2 * CONNECTOR_ANGLE_SCALE);
		}
	}

	// Resolve Anchor Forces
	for(std::vector<AnchoredItems>::iterator anchor = _anchoredItems.begin(); anchor != _anchoredItems.end(); anchor++){
		Phys_Connector* connector = anchor->connector;
		const Geo_Actor* actor = anchor->anchoredItems.first;
		const Vec3f* anchorPos = anchor->anchoredItems.second;

		calcConnectorAttrib(connector, actor->getPosition(), *anchorPos);

		// Forces acting by length displacement
		if(abs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // no forces act unless threshhold exceeded!
			// determines direction of force
			Vec3f force = (connector->length < connector->restLength)
				? actor->getPosition() - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - actor->getPosition(); // length is longer, pull inward
			force.normalize(); // normalizing for directional unit vector

			double lengthDiff = abs(connector->length - connector->restLength); // get how much displacement occured
			const Vec3f forceFinal = force * ((lengthDiff * connector->kVal) * 0.5f);

			addForce(actor->getName(), forceFinal); // adding forces to target actor!
		}
	
		if (connector->restAngleVecn1 != connector->angleVecn1 && connector->restAngleVecn2 != connector->angleVecn2) {
			const Vec3f forceAngle = connector->restAngleVecn1 - connector->angleVecn1;
			addForce(actor->getName(), forceAngle * CONNECTOR_ANGLE_SCALE);
		}
	}

	// Resolve general movement here
	for (std::map<Geo_Actor*, Phys_Actor*>::iterator m = _actorPhys_map.begin(); m != _actorPhys_map.end(); m++) {
		Geo_Actor* targetGeo = m->first;
		Phys_Actor* physActor = m->second;
		
		if (physActor->actingForceCount > 0) 
			for (unsigned forceIndex = 0; forceIndex < physActor->actingForceCount; forceIndex++) {
				physActor->acceleration = physActor->acceleration + (*(physActor->forces + forceIndex)) * (1.0f / physActor->mass);
				*(physActor->forces + forceIndex) = VEC_3F_ZERO; // removing current resolved force
			}
		(physActor->isGravityEnabled) ? physActor->actingForceCount = 1 : physActor->actingForceCount = 0; // reset forces on physics actor
		
		physActor->velocity = physActor->velocity + (physActor->acceleration * elapseSecs); // velocity integration
		physActor->velocity = physActor->velocity * (float)physActor->damping; // velocity damping

		Vec3f updatedPos = (physActor->velocity * (float)elapseSecs) + ((physActor->acceleration * pow(elapseSecs, 2))) * 0.5f;
		targetGeo->updatePos(updatedPos); // position integration

		physActor->acceleration = VEC_3F_ZERO; // resetting acceleration
	}
}