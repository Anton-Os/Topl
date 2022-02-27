#include "Topl_Scene.hpp"

static void error_notFound(const std::string& objTypeStr, const std::string& name) {
	printf("ERROR! Could not find %s object: \n", objTypeStr.c_str());
	perror(name.c_str());
	putchar('\n');
}

static void error_forcesExcess() {
	perror("ERROR! Too many forces on one object!");
	putchar('\n');
}

// Recomputes connector attributes
static void calcConnectorAttrib(Phys_Connector* connector, const Eigen::Vector3f& pos1, const Eigen::Vector3f& pos2){
	Eigen::Vector3f linkDiff = pos1 - pos2;
	connector->length = getVecLength(linkDiff);
	connector->centerPoint = (pos1 + pos2) / 2;
	connector->angle_NVec1 = pos1 - connector->centerPoint;
	connector->angle_NVec1.normalize();
	connector->angle_NVec2 = pos2 - connector->centerPoint;
	connector->angle_NVec2.normalize();
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

void Topl_Scene::addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2) {

	const Geo_Actor* link1 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name1 == (*actor)->getName()) link1 = *actor;
	if (link1 == nullptr) return error_notFound("link geometry", name1); // Error

	const Geo_Actor* link2 = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name2 == (*actor)->getName()) link2 = *actor;
	if (link2 == nullptr) return error_notFound("link geometry", name2);

	if(!connector->getIsPreset()) 
		connector->preset(*link1->getPos(), *link2->getPos()); // presets link data to defaults

	_linkedItems.push_back({connector, std::make_pair(link1, link2)}); // add new link
}

void Topl_Scene::addAnchor(Phys_Connector* connector, const std::string& name, const Eigen::Vector3f* pos) {

	const Geo_Actor* targetActor = nullptr;
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) targetActor = *actor;
	if (targetActor == nullptr) return error_notFound("link geometry", name); // Error

	if(!connector->getIsPreset()) 
		connector->preset(*targetActor->getPos(), *pos); // presets anchor data to defaults

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

		calcConnectorAttrib(connector, *linkItem1->getPos(), *linkItem2->getPos());

		// Forces acting by length displacement
		if(abs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // No forces unless threshhold exceeded!
			// determines direction of force for first geometry link
			Eigen::Vector3f force1 = (connector->length < connector->restLength)
				? *(linkItem1->getPos()) - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - *(linkItem1->getPos()); // length is longer, pull inward
			force1.norm(); // normalizing for directional unit vector

			// determines direction of force for second geometry link
			Eigen::Vector3f force2 = (connector->length < connector->restLength)
				? *(linkItem2->getPos()) - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - *(linkItem2->getPos()); // length is longer, pull inward
			force2.norm(); // normalizing for directional unit vector

			double lengthDiff = abs(connector->length - connector->restLength); // get how much displacement occured
			const Eigen::Vector3f forceFinal1 = force1 * ((lengthDiff * connector->kVal) * 0.5f);
			const Eigen::Vector3f forceFinal2 = force2 * ((lengthDiff * connector->kVal) * 0.5f);

			addForce(linkItem1->getName(), forceFinal1); // add force to first actor
			addForce(linkItem2->getName(), forceFinal2); // add force to second actor
		}

		// Forces acting by angular displacement // TODO: Add a Threshold value!!!
		if (connector->restAngle_NVec1 != connector->angle_NVec1 && connector->restAngle_NVec2 != connector->angle_NVec2) {
			const Eigen::Vector3f forceAngle1 = connector->restAngle_NVec1 - connector->angle_NVec1;
			const Eigen::Vector3f forceAngle2 = connector->restAngle_NVec2 - connector->angle_NVec2;

			addForce(linkItem1->getName(), forceAngle1 * CONNECTOR_ANGLE_SCALE);
			addForce(linkItem2->getName(), forceAngle2 * CONNECTOR_ANGLE_SCALE);
		}
	}

	// Resolve Anchor Forces
	for(std::vector<AnchoredItems>::iterator anchor = _anchoredItems.begin(); anchor != _anchoredItems.end(); anchor++){
		Phys_Connector* connector = anchor->connector;
		const Geo_Actor* actor = anchor->anchoredItems.first;
		const Eigen::Vector3f* anchorPos = anchor->anchoredItems.second;

		calcConnectorAttrib(connector, *actor->getPos(), *anchorPos);

		// Forces acting by length displacement
		if(abs(connector->length - connector->restLength) > CONNECTOR_LEN_THRESH){ // no forces act unless threshhold exceeded!
			// determines direction of force
			Eigen::Vector3f force = (connector->length < connector->restLength)
				? *(actor->getPos()) - connector->centerPoint // length is shorter, push outward
				: connector->centerPoint - *(actor->getPos()); // length is longer, pull inward
			force.norm(); // normalizing for directional unit vector

			double lengthDiff = abs(connector->length - connector->restLength); // get how much displacement occured
			const Eigen::Vector3f forceFinal = force * ((lengthDiff * connector->kVal) * 0.5f);

			addForce(actor->getName(), forceFinal); // adding forces to target actor!
		}
	
		if (connector->restAngle_NVec1 != connector->angle_NVec1 && connector->restAngle_NVec2 != connector->angle_NVec2) {
			const Eigen::Vector3f forceAngle = connector->restAngle_NVec1 - connector->angle_NVec1;
			addForce(actor->getName(), forceAngle * CONNECTOR_ANGLE_SCALE);
		}
	}

	// Resolve general movement here
	for (std::map<Geo_Actor*, Phys_Actor*>::iterator m = _actorPhys_map.begin(); m != _actorPhys_map.end(); m++) {
		Geo_Actor* targetGeo = m->first;
		Phys_Actor* physActor = m->second;
		
		if (physActor->actingForceCount > 0) 
			for (unsigned forceIndex = 0; forceIndex < physActor->actingForceCount; forceIndex++) {
				physActor->acceleration += (*(physActor->forces + forceIndex)) / physActor->mass;
				*(physActor->forces + forceIndex) = Eigen::Vector3f(0.0, 0.0, 0.0); // removing current resolved force
			}
		(physActor->isGravityEnabled) ? physActor->actingForceCount = 1 : physActor->actingForceCount = 0; // reset forces on physics actor
		
		physActor->velocity += (physActor->acceleration) * elapseSecs; // velocity integration
		physActor->velocity *= physActor->damping; // velocity damping

		targetGeo->updatePos((physActor->velocity * elapseSecs) + 0.5 * (physActor->acceleration * pow(elapseSecs, 2))); // position integration

		physActor->acceleration = Eigen::Vector3f(0.0, 0.0, 0.0); // resetting acceleration
	}
}