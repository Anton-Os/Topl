#include "Topl_Scene.hpp"

#ifdef TOPL_ENABLE_PHYSICS

// recomputes connector attributes
static void calcConnectorAttrib(Phys_Connector* connector, const Vec3f& pos1, const Vec3f& pos2){
	Vec3f linkDiff = Vec3f(pos1) - Vec3f(pos2);
	connector->length = linkDiff.len();
	// if(connector->restLength < 0.0) connector->restLength = connector->length; // if bad rest length then fix
	connector->centerPoint = (Vec3f(pos1) + Vec3f(pos2)) * 0.5f;
	connector->angleVecN1 = Vec3f(pos1) - connector->centerPoint;
	connector->angleVecN1.normalize();
	connector->angleVecN2 = Vec3f(pos2) - connector->centerPoint;
	connector->angleVecN2.normalize();
}

// Scene Dynamics

void Topl_Scene::addPhysics(const std::string& name, Phys_Actor* physActor) {
	for(std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if(name == (*actor)->getName()){
			_physicsMap.erase(*actor);
			_physicsMap.insert({ *actor, physActor });
		}
}

void Topl_Scene::addForce(const std::string& name, const Vec3f& forceVec) {
    addForces(name, forceVec, VEC_3F_ZERO, VEC_3F_ZERO);
}

void Topl_Scene::addForces(const std::string& name, const Vec3f& posVec, const Vec3f& angleVec, const Vec3f& sizeVec) {
    for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++) // TODO: Replace with find
        if (name == (*actor)->getName()) {
            if (_physicsMap.find(*actor) == _physicsMap.end())
                return logMessage(MESSAGE_Exclaim, "Could not locate physics actor for object: " + name);

            Phys_Actor* physActor = _physicsMap.find(*actor)->second;
            if(!posVec.isZero()) physActor->addForce(FORCE_Directional, posVec);
            if(!angleVec.isZero()){
                physActor->addForce(FORCE_Angular, angleVec);
                if(physActor->angle.isZero()) physActor->angle = *(*actor)->getRot(); // set the angle to revert to
            }
            if(!sizeVec.isZero()){
                physActor->addForce(FORCE_Constricting, sizeVec);
                if(physActor->scale.isZero()) physActor->scale = *(*actor)->getSize(); // set the scale to revert to
            }
            // break;
        }
}

void Topl_Scene::addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2) {
	const Geo_Actor* link1 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name1 == (*actor)->getName()) link1 = *actor;
	if (link1 == nullptr) return logMessage(MESSAGE_Exclaim, "Could not locate actor to link for object: " + name1);

	const Geo_Actor* link2 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name2 == (*actor)->getName()) link2 = *actor;
	if (link2 == nullptr) return logMessage(MESSAGE_Exclaim, "Could not locate actor to link for object: " + name2);

	_linkedItems.push_back({ connector, std::make_pair(link1, link2)}); // add new link
}

void Topl_Scene::addAnchor(Phys_Connector* connector, const std::string& name, const Vec3f* pos) {
	const Geo_Actor* targetActor = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) targetActor = *actor;
	if (targetActor == nullptr) return logMessage(MESSAGE_Exclaim, "Could not locate actor to anchor for object: " + name);

	_anchoredItems.push_back({ connector, std::make_pair(targetActor, pos)}); // add new anchor
}


void Topl_Scene::remConnector(const std::string& targetName){
	for(std::vector<LinkedItems>::iterator link = _linkedItems.begin(); link != _linkedItems.end(); link++)
		if(link->linkedItems.first->getName() == targetName || link->linkedItems.second->getName() == targetName)
			_linkedItems.erase(link);

	for(std::vector<AnchoredItems>::iterator anchor = _anchoredItems.begin(); anchor != _anchoredItems.end(); anchor++)
		if(anchor->anchoredItems.first->getName() == targetName)
			_anchoredItems.erase(anchor);
}


void Topl_Scene::resolvePhysics(FORCE_Type type) {
	double elapseSecs = _ticker.getRelSecs();
    // Resolve Link Connections // TODO: Run in a thread
	if(elapseSecs < PHYS_CALC_THRESH) // adding threshhold value
	for(std::vector<LinkedItems>::iterator link = _linkedItems.begin(); link != _linkedItems.end(); link++){
		Phys_Connector* connector = link->connector;
		const Geo_Actor* linkItem1 = link->linkedItems.first;
		const Geo_Actor* linkItem2 = link->linkedItems.second;

		calcConnectorAttrib(connector, *linkItem1->getPos(), *linkItem2->getPos());

        // Forces acting by length displacement
		if(fabs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // No forces unless threshhold exceeded!
			// determines direction of force for first geometry link
			Vec3f force1 = (connector->length < connector->restLength)
				? *linkItem1->getPos() - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - *linkItem1->getPos(); // length is longer, pull inward
			force1.normalize(); // normalizing for directional unit vector

			// determines direction of force for second geometry link
			Vec3f force2 = (connector->length < connector->restLength)
				? *linkItem2->getPos() - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - *linkItem2->getPos(); // length is longer, pull inward
			force2.normalize(); // normalizing for directional unit vector

			double lengthDiff = fabs(connector->length - connector->restLength); // get how much displacement occured
			const Vec3f forceFinal1 = force1 * ((lengthDiff * connector->kVal) * 0.5f);
			const Vec3f forceFinal2 = force2 * ((lengthDiff * connector->kVal) * 0.5f);

			addForce(linkItem1->getName(), forceFinal1); // add force to first actor
			addForce(linkItem2->getName(), forceFinal2); // add force to second actor
		}

		// Forces acting by angular displacement // TODO: Add a Threshold value!!!
		if (connector->restAngleVecN1 != connector->angleVecN1 && connector->restAngleVecN2 != connector->angleVecN2) {
			const Vec3f forceAngle1 = connector->restAngleVecN1 - connector->angleVecN1;
			const Vec3f forceAngle2 = connector->restAngleVecN2 - connector->angleVecN2;

			addForce(linkItem1->getName(), forceAngle1 * CONNECTOR_ANGLE_SCALE);
			addForce(linkItem2->getName(), forceAngle2 * CONNECTOR_ANGLE_SCALE);
		}
	}

    // Resolve Anchor Connections // TODO: Run in a thread
	if(elapseSecs < PHYS_CALC_THRESH) // adding threshhold value
	for(std::vector<AnchoredItems>::iterator anchor = _anchoredItems.begin(); anchor != _anchoredItems.end(); anchor++){
		Phys_Connector* connector = anchor->connector;
		const Geo_Actor* actor = anchor->anchoredItems.first;
		const Vec3f* anchorPos = anchor->anchoredItems.second;

		calcConnectorAttrib(connector, *actor->getPos(), *anchorPos);

		// Forces acting by length displacement
		if(fabs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // no forces act unless threshhold exceeded!
			// determines direction of force
			Vec3f force = (connector->length < connector->restLength)
				? *actor->getPos() - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - *actor->getPos(); // length is longer, pull inward
			force.normalize(); // normalizing for directional unit vector

			double lengthDiff = fabs(connector->length - connector->restLength); // get how much displacement occured
			const Vec3f forceFinal = force * ((lengthDiff * connector->kVal) * 0.5f);

			addForce(actor->getName(), forceFinal); // adding forces to target actor!
		}
	
		if (connector->restAngleVecN1 != connector->angleVecN1 && connector->restAngleVecN2 != connector->angleVecN2) {
			const Vec3f forceAngle = connector->restAngleVecN1 - connector->angleVecN1;
			addForce(actor->getName(), forceAngle * CONNECTOR_ANGLE_SCALE);
		}
	}

    // Resolve All Forces // TODO: Run in a thread
	for (std::map<Geo_Actor*, Phys_Actor*>::iterator m = _physicsMap.begin(); m != _physicsMap.end(); m++) {
		Geo_Actor* geoActor = m->first;
		Phys_Actor* physActor = m->second;

        if (physActor->actingForceCount > 0){
			for (unsigned forceIndex = 0; forceIndex < physActor->actingForceCount; forceIndex++) {
                switch(physActor->getForceAt(forceIndex).type){
                    case FORCE_Directional: physActor->acceleration = physActor->acceleration + physActor->getForceVecAt(forceIndex) * (1.0f / physActor->mass); break;
                    case FORCE_Angular: physActor->angularAcceleration = physActor->angularAcceleration + physActor->getForceVecAt(forceIndex) * (1.0f / physActor->mass); break;
                    case FORCE_Constricting: physActor->scaleAcceleration = physActor->scaleAcceleration + physActor->getForceVecAt(forceIndex) * (1.0f / physActor->mass); break;
                }
                // physActor->acceleration = physActor->acceleration + physActor->getForceVecAt(forceIndex) * (1.0f / physActor->mass); break;
                (*(physActor->actingForces + forceIndex)).force = VEC_3F_ZERO; // removing current resolved force
			}
            physActor->actingForceCount = 0; // TODO: account for gravity
            // (physActor->isGravityEnabled) ? physActor->actingForceCount = 1 : physActor->actingForceCount = 0; // reset forces on physics actor
        }

		switch(type){
			case FORCE_Directional:
				geoActor->updatePos(physActor->integrate(FORCE_Directional, elapseSecs));
			break;
			case FORCE_Angular:
				if(physActor->angularVelocity == VEC_3F_ZERO) physActor->angle = *(geoActor->getRot());
        		geoActor->updateRot(physActor->integrate(FORCE_Angular, elapseSecs));

				if(*(geoActor->getRot()) != physActor->angle) // Balancing Force
					physActor->angularAcceleration = (physActor->angle - *(geoActor->getRot())) * (1.0f / physActor->mass) * (physActor->damping * 0.5);
			break;
			case FORCE_Constricting:
				if(physActor->scaleVelocity == VEC_3F_ZERO) physActor->scale = *(geoActor->getSize());
				geoActor->updateSize(physActor->integrate(FORCE_Constricting, elapseSecs));

				if(*(geoActor->getSize()) != physActor->scale) // Balancing Force
					physActor->scaleAcceleration = (physActor->scale - *(geoActor->getSize())) * (1.0f / physActor->mass) * (physActor->damping * 0.5);
			break;
		}
	}
}

#endif
