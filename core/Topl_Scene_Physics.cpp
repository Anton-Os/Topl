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

void Topl_Scene::addPhysics(const std::string& name, Phys_Properties* pProp) {
	// Find matching geometry component
	for(std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if((*currentGeo)->getName() == name){
			mGeoPhys_map.insert({ *currentGeo, pProp });
			return;
		}

	error_notFound("geometry", name); // Error
	return;
}

void Topl_Scene::addForce(const std::string& name, const Eigen::Vector3f& forceVec) {
	// Find matching geometry component
	for (std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if (name == (*currentGeo)->getName()) {
			if (mGeoPhys_map.find(*currentGeo) == mGeoPhys_map.end()) return error_notFound("physics", name); // Error

			Phys_Properties* targetPhys = mGeoPhys_map.find(*currentGeo)->second;
			vec3f_cptr targetPos = (*currentGeo)->getPos();

			if(!targetPhys->addForce(forceVec)) return error_forcesExcess(); // Error
		}

	error_notFound("geometry or physics", name); // Error
	return;
}

void Topl_Scene::addConnector(Phys_Connector* connector, const std::string& name1, const std::string& name2) {

	const Geo_Component* link1 = nullptr;
	for (std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if (name1 == (*currentGeo)->getName()) link1 = *currentGeo;

	if (link1 == nullptr) return error_notFound("link geometry", name1); // Error

	const Geo_Component* link2 = nullptr;
	for (std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if (name2 == (*currentGeo)->getName()) link2 = *currentGeo;

	if (link2 == nullptr) return error_notFound("link geometry", name2);


	vec3f_cptr pos1 = link1->getPos();
	vec3f_cptr pos2 = link2->getPos();

	// Compute the length of the distance between both vectors
	Eigen::Vector3f diffPos = *pos1 - *pos2;
	connector->length = sqrt(pow(diffPos.x(), 2) + pow(diffPos.y(), 2) + pow(diffPos.z(), 2));
	// FOR NOW make length and rest length the same value, therefore no force would act on the linked items
	connector->restLength = connector->length;
	// Compute the center point which needs to be updated in the resolvePhysics() method
	connector->centerPoint = (*pos1 + *pos2) / 2;

	// Compute the INITIAL, NORMALIZED angle vector between the center and linked items
	connector->restAngleNormVec1 = *pos1 - connector->centerPoint;
	connector->restAngleNormVec1.normalize();
	connector->restAngleNormVec2 = *pos2 - connector->centerPoint;
	connector->restAngleNormVec2.normalize();

	// Add the new linked items to the scene manager data
	mLinkedItems.push_back({connector, std::make_pair(link1, link2)});
}

void Topl_Scene::modConnector(const std::string& targetName, Eigen::Vector3f rotAnglesVec, double lengthScale) {
	for(std::vector<LinkedItems>::iterator currentLink = mLinkedItems.begin(); currentLink != mLinkedItems.end(); currentLink++)
		if(currentLink->linkedItems.first->getName() == targetName || currentLink->linkedItems.second->getName() == targetName){
			
			currentLink->connector->restLength *= lengthScale;
			
			currentLink->connector->restAngleNormVec1 = rotAnglesVec; // TODO: Make this rotate!
			currentLink->connector->restAngleNormVec1.normalize();
			currentLink->connector->restAngleNormVec2 = -1 * rotAnglesVec; // TODO: Make this rotate!
			currentLink->connector->restAngleNormVec2.normalize();
		}
}

void Topl_Scene::remConnector(const std::string& targetName){
	for(std::vector<LinkedItems>::iterator currentLink = mLinkedItems.begin(); currentLink != mLinkedItems.end(); currentLink++)
		if(currentLink->linkedItems.first->getName() == targetName || currentLink->linkedItems.second->getName() == targetName)
			mLinkedItems.erase(currentLink);
}


void Topl_Scene::resolvePhysics() {
	double physElapseSecs = mPhysTicker.getRelSecs();

	// Resolve connector and link forces here and general computations
	for(std::vector<LinkedItems>::iterator currentLink = mLinkedItems.begin(); currentLink != mLinkedItems.end(); currentLink++){
		Phys_Connector* connector = currentLink->connector;
		const Geo_Component* linkItem1 = currentLink->linkedItems.first;
		const Geo_Component* linkItem2 = currentLink->linkedItems.second;

		connector->length = sqrt( // Length needs to be updated
			pow(linkItem1->getPos()->x() - linkItem2->getPos()->x(), 2)
			+ pow(linkItem1->getPos()->y() - linkItem2->getPos()->y(), 2)
			+ pow(linkItem1->getPos()->z() - linkItem2->getPos()->z(), 2)
		);

		connector->centerPoint = (*linkItem1->getPos() + *linkItem2->getPos()) / 2;
		connector->angleNormVec1 = *linkItem1->getPos() - connector->centerPoint;
		connector->angleNormVec1.normalize();
		connector->angleNormVec2 = *linkItem2->getPos() - connector->centerPoint;
		connector->angleNormVec2.normalize();
 
		// Forces acting by length displacement
		if(abs(connector->length - connector->restLength) > TOPL_CONNECTOR_LEN_THRESH){ // No forces unless threshhold exceeded!
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
		if (connector->restAngleNormVec1 != connector->angleNormVec1 && connector->restAngleNormVec2 != connector->angleNormVec2) {
			const Eigen::Vector3f forceAngle1 = connector->restAngleNormVec1 - connector->angleNormVec1;
			const Eigen::Vector3f forceAngle2 = connector->restAngleNormVec2 - connector->angleNormVec2;

			addForce(linkItem1->getName(), forceAngle1 * TOPL_CONNECTOR_ANGLE_MULT);
			addForce(linkItem2->getName(), forceAngle2 * TOPL_CONNECTOR_ANGLE_MULT);
		}
	}

	// Resolve general movement here
	for (std::map<Geo_Component*, Phys_Properties*>::iterator currentMap = mGeoPhys_map.begin(); currentMap != mGeoPhys_map.end(); currentMap++) {
		Geo_Component* targetGeo = currentMap->first;
		Phys_Properties* physProps = currentMap->second;
		
		if (physProps->actingForceCount > 0) 
			for (unsigned forceIndex = 0; forceIndex < physProps->actingForceCount; forceIndex++) {
				physProps->acceleration += (*(physProps->forces + forceIndex)) / physProps->mass;
				*(physProps->forces + forceIndex) = Eigen::Vector3f(0.0, 0.0, 0.0); // Disabling current force
			}
		(physProps->isGravityEnabled) ? physProps->actingForceCount = 1 : physProps->actingForceCount = 0; // We have resolved all the forces, resetting force count
		
		// Velocity Integrator
		physProps->velocity += (physProps->acceleration) * physElapseSecs; // Division converts elapsed time to seconds from milliseconds
		// Velocity Damping, to avoid infinite displacement
		physProps->velocity *= physProps->damping;

		// Position integrator
		targetGeo->updatePos((physProps->velocity * physElapseSecs) + 0.5 * (physProps->acceleration * pow(physElapseSecs, 2))); // For now not factoring in acceleration

		physProps->acceleration = Eigen::Vector3f(0.0, 0.0, 0.0); // Resetting acceleration
	}
}