#include "Topl_SceneManager.hpp"

static void print_ObjNotFound(const std::string& objTypeStr, const std::string& name) {
	printf("Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

// Scene Manager implementation code

unsigned Geo_Component::mId_count = 0;

topl_geoComponent_cptr Topl_SceneManager::getGeoNode(unsigned index) const {
	if (mIdToGeo_map.find(index) == mIdToGeo_map.end()) {
		puts("Index for geometry provided cannot be found");
		return nullptr;
	}

	return mIdToGeo_map.at(index);
}

topl_geoComponent_cptr Topl_SceneManager::getGeoNode(const std::string& name) const {
	if (mNameToId_map.find(name) == mNameToId_map.end()) {
		print_ObjNotFound("geometry", name);
		return nullptr;
	}

	unsigned gIndex = mNameToId_map.at(name);
	return mIdToGeo_map.at(gIndex);
}

void Topl_SceneManager::addGeometry(const std::string& name, Geo_Component* geoNode) {
	if (mNameToId_map.find(name) != mNameToId_map.end()) {
		puts("Overriding geometry object:");
		puts(name.c_str());
	}

	geoNode->setName(name);

	mNameToId_map.insert({ name, ((Geo_Component*)geoNode)->getId() });
	mIdToGeo_map.insert({ ((Geo_Component*)geoNode)->getId(), geoNode });
}

void Topl_SceneManager::addPhysics(const std::string& name, Phys_Properties* pProp) {
	if (mNameToId_map.find(name) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name);

	mIdToPhysProp_map.insert({ mNameToId_map.at(name), pProp });
}

void Topl_SceneManager::addForce(const std::string& name, const Eigen::Vector3f& vec) {
	if (mNameToId_map.find(name) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name);

	Geo_Component* targetNode = mIdToGeo_map.at(mNameToId_map.at(name));
	vec3f_cptr targetPos = targetNode->getPos();

	if (mIdToPhysProp_map.find(mNameToId_map.at(name)) == mIdToPhysProp_map.end())
		return print_ObjNotFound("physics", name);

	Phys_Properties* targetPhys = mIdToPhysProp_map.at(mNameToId_map.at(name));

	if (targetPhys->actingForceCount < MAX_PHYS_FORCES) {
		*(targetPhys->forces + targetPhys->actingForceCount) = vec;
		targetPhys->actingForceCount++; // Moves to the next available force space
	}
	else
		return; // Too many forces are acting on the object simultaneously
}

void Topl_SceneManager::addConnector(Phys_Connector* connector, const std::string& name1, const std::string& name2) {
	// TODO: Make sure a duplicate connector is not encountered in the mLinkedItems vector

	if (mNameToId_map.find(name1) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name1);

	if (mNameToId_map.find(name2) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name2);

	LinkedItems items = { // Could instead be implemented as as a LinkedItems constructor
			connector,
			std::make_pair(
				mIdToGeo_map.at(mNameToId_map.at(name1)),
				mIdToGeo_map.at(mNameToId_map.at(name2))
			)
	};

	const Geo_Component* link1 = items.linkedItems.first;
	const Geo_Component* link2 = items.linkedItems.second;

	vec3f_cptr pos1 = link1->getPos();
	vec3f_cptr pos2 = link2->getPos();

	// Compute the length of the distance between both vectors
	Eigen::Vector3f diffPos = *pos1 - *pos2;
	connector->length = sqrt(pow(diffPos.x(), 2) + pow(diffPos.y(), 2) + pow(diffPos.z(), 2));

	// FOR NOW make length and rest length the same value, therefore no force would act on the linked items
	connector->restLength = connector->length;

	// Compute the center point which needs to be updated in the resolvePhysics() method
	connector->centerPoint = (*pos1 + *pos2) / 2;

	// Add the new linked items to the scene manager data
	mLinkedItems.push_back(items);
}

void Topl_SceneManager::resolvePhysics() {
	double physElapseMils = mPhysTicker.getRelMillsecs();
	double physElapseSecs = physElapseMils / 1000.0;

	// Resolve connector forces here
	for(std::vector<LinkedItems>::iterator currentLink = mLinkedItems.begin(); currentLink != mLinkedItems.end(); currentLink++){
		Phys_Connector* connector = currentLink->connector;
		const Geo_Component* linkItem1 = currentLink->linkedItems.first;
		const Geo_Component* linkItem2 = currentLink->linkedItems.second;

		if (connector->length != connector->restLength) { // No forces occur if the length and rest length match
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
		}
	}

	for (std::map<unsigned, Phys_Properties*>::iterator physCurrentMap = mIdToPhysProp_map.begin(); physCurrentMap != mIdToPhysProp_map.end(); physCurrentMap++) {
		Phys_Properties* physProps = physCurrentMap->second;
		
		Geo_Component* targetNode = mIdToGeo_map.at(physCurrentMap->first); // Gets the geometry node bound to the physics object
		
		if (physProps->actingForceCount > 0) {
			for (unsigned forceIndex = 0; forceIndex < physProps->actingForceCount; forceIndex++)
				physProps->acceleration += (*(physProps->forces + forceIndex)) / physProps->mass;
		}
		// Velocity Integrator,
		physProps->velocity += (physProps->acceleration) * physElapseSecs; // Division converts elapsed time to seconds from milliseconds
		// Velocity Damping, to avoid infinite displacement
		physProps->velocity *= physProps->damping;

		// Position integrator
		targetNode->updatePos((physProps->velocity * physElapseSecs) + 0.5 * (physProps->acceleration * pow(physElapseSecs, 2))); // For now not factoring in acceleration

		physProps->acceleration = Eigen::Vector3f(0.0, 0.0, 0.0); // Resetting acceleration
	}
}

#ifdef RASTERON_H
const Rasteron_Image* Topl_SceneManager::getFirstTexture(unsigned index) const {
	for (unsigned t = 0; t < mIdToTex.size(); t++)
		if (index == mIdToTex.at(t).first)
			return mIdToTex.at(t).second;
}

unsigned Topl_SceneManager::getTextures(unsigned index, const Rasteron_Image** images) const {
	// Dummy check if textures exist at all.
	unsigned texCount = 0;

	for (unsigned t = 0; t < mIdToTex.size(); t++)
		if (index == mIdToTex.at(t).first) texCount++;

	// Check if images has members
	if (images == nullptr) return texCount;

	unsigned iOffset = 0;
	for (unsigned t = 0; t < mIdToTex.size(); t++)
		if (index == mIdToTex.at(t).first) {
			*(images + iOffset) = mIdToTex.at(t).second;
			iOffset++;
		}
}
#endif