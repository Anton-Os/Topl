#include "Topl_Scene.hpp"

static void error_notFound(const std::string& objTypeStr, const std::string& name) {
	printf("ERROR! Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

static void error_forcesExcess() {
	puts("ERROR! Too many forces on one object!");
	putchar('\n');
}

// Scene Builder implementation code

void Topl_Scene::addPhysics(const std::string& name, Phys_Actor* physActor) {
	// Find matching geometry component
	for(std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if((*actor)->getName() == name){
			_actorPhys_map.insert({ *actor, physActor });
			return;
		}

	error_notFound("geometry", name); // Error
	return;
}

void Topl_Scene::addForce(const std::string& name, const Eigen::Vector3f& forceVec) {
	// Find matching geometry component
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			if (_actorPhys_map.find(*actor) == _actorPhys_map.end()) return error_notFound("physics", name); // Error

			Phys_Actor* physActor = _actorPhys_map.find(*actor)->second;
			vec3f_cptr targetPos = (*actor)->getPos();

			if(!physActor->addForce(forceVec)) return error_forcesExcess(); // Error
		}
}

void Topl_Scene::addConnector(Phys_Connector* connector, const std::string& name1, const std::string& name2) {

	const Geo_Actor* link1 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name1 == (*actor)->getName()) link1 = *actor;

	if (link1 == nullptr) return error_notFound("link geometry", name1); // Error

	const Geo_Actor* link2 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name2 == (*actor)->getName()) link2 = *actor;

	if (link2 == nullptr) return error_notFound("link geometry", name2);

	if(!connector->getIsPreset()){
		vec3f_cptr pos1 = link1->getPos();
		vec3f_cptr pos2 = link2->getPos();

		// Compute the length of the distance between both vectors
		Eigen::Vector3f diffPos = *pos1 - *pos2;
		connector->length = sqrt(pow(diffPos.x(), 2) + pow(diffPos.y(), 2) + pow(diffPos.z(), 2));
		// FOR NOW make length and rest length the same value, therefore no force would act on the linked items
		connector->restLength = connector->length;
		// Compute the center point which needs to be updated in the resolvePhysics() method
		connector->centerPoint = (*pos1 + *pos2) / 2;

		// Compute the INITIAL NORMALIZED angle vector between the center and linked items
		connector->restAngle_NVec1 = *pos1 - connector->centerPoint;
		connector->restAngle_NVec1.normalize();
		connector->restAngle_NVec2 = *pos2 - connector->centerPoint;
		connector->restAngle_NVec2.normalize();
	}

	// Add the new linked items to the scene manager data
	_linkedItems.push_back({connector, std::make_pair(link1, link2)});
}

void Topl_Scene::modConnector(const std::string& targetName, Eigen::Vector3f rotAnglesVec, double lengthScale) {
	for(std::vector<LinkedItems>::iterator currentLink = _linkedItems.begin(); currentLink != _linkedItems.end(); currentLink++)
		if(currentLink->linkedItems.first->getName() == targetName || currentLink->linkedItems.second->getName() == targetName){
			
			currentLink->connector->restLength *= lengthScale;
			
			currentLink->connector->restAngle_NVec1 = rotAnglesVec; // TODO: Make this rotate!
			currentLink->connector->restAngle_NVec1.normalize();
			currentLink->connector->restAngle_NVec2 = -1 * rotAnglesVec; // TODO: Make this rotate!
			currentLink->connector->restAngle_NVec2.normalize();
		}
}

void Topl_Scene::remConnector(const std::string& targetName){
	for(std::vector<LinkedItems>::iterator currentLink = _linkedItems.begin(); currentLink != _linkedItems.end(); currentLink++)
		if(currentLink->linkedItems.first->getName() == targetName || currentLink->linkedItems.second->getName() == targetName)
			_linkedItems.erase(currentLink);
}


void Topl_Scene::resolvePhysics() {
	double physElapseSecs = _physTicker.getRelSecs();

	// Resolve connector and link forces here and general computations
	for(std::vector<LinkedItems>::iterator currentLink = _linkedItems.begin(); currentLink != _linkedItems.end(); currentLink++){
		Phys_Connector* connector = currentLink->connector;
		const Geo_Actor* linkItem1 = currentLink->linkedItems.first;
		const Geo_Actor* linkItem2 = currentLink->linkedItems.second;

		connector->length = sqrt( // Length needs to be updated
			pow(linkItem1->getPos()->x() - linkItem2->getPos()->x(), 2)
			+ pow(linkItem1->getPos()->y() - linkItem2->getPos()->y(), 2)
			+ pow(linkItem1->getPos()->z() - linkItem2->getPos()->z(), 2)
		);

		connector->centerPoint = (*linkItem1->getPos() + *linkItem2->getPos()) / 2;
		connector->angle_NVec1 = *linkItem1->getPos() - connector->centerPoint;
		connector->angle_NVec1.normalize();
		connector->angle_NVec2 = *linkItem2->getPos() - connector->centerPoint;
		connector->angle_NVec2.normalize();
 
		// Forces acting by length displacement
		if(abs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // No forces unless threshhold exceeded!
			// Determines direction of force for first geometry link
			Eigen::Vector3f forceDirection1 = (connector->length < connector->restLength)
				? *(linkItem1->getPos()) - connector->centerPoint // Outward push because length is shorter
				: connector->centerPoint - *(linkItem1->getPos()); // Inward pull because length is longer
			forceDirection1.norm(); // Normalizing for directional unit vector

			// Determines direction of force for second geometry link
			Eigen::Vector3f forceDirection2 = (connector->length < connector->restLength)
				? *(linkItem2->getPos()) - connector->centerPoint // Outward push because length is shorter
				: connector->centerPoint - *(linkItem2->getPos()); // Inward pull because length is longer
			forceDirection2.norm(); // Normalizing for directional unit vector

			double lengthDiff = abs(connector->length - connector->restLength); // Get how much displacement occured
			
			const Eigen::Vector3f forceFinal1 = forceDirection1 * ((lengthDiff * connector->kVal) * 0.5f);
			const Eigen::Vector3f forceFinal2 = forceDirection2 * ((lengthDiff * connector->kVal) * 0.5f);

			// Adding forces to target objects, most important step!
			addForce(linkItem1->getName(), forceFinal1);
			addForce(linkItem2->getName(), forceFinal2);
		}

		// Forces acting by angular displacement // TODO: Add a Threshold value!!!
		if (connector->restAngle_NVec1 != connector->angle_NVec1 && connector->restAngle_NVec2 != connector->angle_NVec2) {
			const Eigen::Vector3f forceAngle1 = connector->restAngle_NVec1 - connector->angle_NVec1;
			const Eigen::Vector3f forceAngle2 = connector->restAngle_NVec2 - connector->angle_NVec2;

			addForce(linkItem1->getName(), forceAngle1 * CONNECTOR_ANGLE_SCALE);
			addForce(linkItem2->getName(), forceAngle2 * CONNECTOR_ANGLE_SCALE);
		}
	}

	// Resolve general movement here
	for (std::map<Geo_Actor*, Phys_Actor*>::iterator m = _actorPhys_map.begin(); m != _actorPhys_map.end(); m++) {
		Geo_Actor* targetGeo = m->first;
		Phys_Actor* physActor = m->second;
		
		if (physActor->actingForceCount > 0) 
			for (unsigned forceIndex = 0; forceIndex < physActor->actingForceCount; forceIndex++) {
				physActor->acceleration += (*(physActor->forces + forceIndex)) / physActor->mass;
				*(physActor->forces + forceIndex) = Eigen::Vector3f(0.0, 0.0, 0.0); // Disabling current force
			}
		(physActor->isGravityEnabled) ? physActor->actingForceCount = 1 : physActor->actingForceCount = 0; // We have resolved all the forces, resetting force count
		
		// Velocity integrator
		physActor->velocity += (physActor->acceleration) * physElapseSecs; // Division converts elapsed time to seconds from milliseconds
		// Velocity damping, to avoid infinite displacement
		physActor->velocity *= physActor->damping;

		// Position integrator
		targetGeo->updatePos((physActor->velocity * physElapseSecs) + 0.5 * (physActor->acceleration * pow(physElapseSecs, 2))); // For now not factoring in acceleration

		physActor->acceleration = Eigen::Vector3f(0.0, 0.0, 0.0); // Resetting acceleration
	}
}