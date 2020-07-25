#include "Topl_SceneManager.hpp"

static void print_ObjNotFound(const std::string& objTypeStr, const std::string& name) {
	printf("Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

// Node implementation code

void Topl_GeoNode::updatePos(Eigen::Vector3f vec) {
	mRelWorldPos += vec;
	for (unsigned c = 0; c < getChildCount(); c++)
		((Topl_GeoNode*)getChild(c))->updatePos(vec); // Move all children as well
}


// Scene Manager implementation code

unsigned Topl_Node::mId_count = 0;

tpl_gEntity_cptr Topl_SceneManager::getGeoNode(unsigned index) const {
	if (mIdToGeo_map.find(index) == mIdToGeo_map.end()) {
		puts("Index for geometry provided cannot be found");
		return nullptr;
	}

	return mIdToGeo_map.at(index);
}

tpl_gEntity_cptr Topl_SceneManager::getGeoNode(const std::string& name) const {
	if (mNameToId_map.find(name) == mNameToId_map.end()) {
		print_ObjNotFound("geometry", name);
		return nullptr;
	}

	unsigned gIndex = mNameToId_map.at(name);
	return mIdToGeo_map.at(gIndex);
}

void Topl_SceneManager::addGeometry(const std::string& name, Topl_GeoNode* geoNode) {
	if (mNameToId_map.find(name) != mNameToId_map.end()) {
		puts("Overriding geometry object:");
		puts(name.c_str());
	}

	geoNode->setName(name);


	std::string targetName = (geoNode->getParent() == nullptr) ? name : "_parent_child"; // TODO: Fix the association

	mNameToId_map.insert({ targetName, ((Topl_Node*)geoNode)->getId() });
	mIdToGeo_map.insert({ ((Topl_Node*)geoNode)->getId(), geoNode });
}

void Topl_SceneManager::addPhysics(const std::string& name, Phys_Properties* pProp) {
	if (mNameToId_map.find(name) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name);

	mIdToPhysProp_map.insert({ mNameToId_map.at(name), pProp });
}

void Topl_SceneManager::addForce(const std::string& name, const Eigen::Vector3f& vec) {
	if (mNameToId_map.find(name) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name);

	Topl_GeoNode* targetNode = mIdToGeo_map.at(mNameToId_map.at(name));
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

	// targetNode->updatePos(vec); // This code needs to be in the resolve physics method!!! REMOVE AND FIX
}

void Topl_SceneManager::resolvePhysics() {
	double physElapseMils = mPhysTicker.getRelMillsecs();
	double physElapseSecs = physElapseMils / 1000.0;

	const double magnifier = 1 / physElapseSecs; // Makes acceleration and force sync with each second!
	// const double magnifier = 1.0;

	for (std::map<unsigned, Phys_Properties*>::iterator physCurrentMap = mIdToPhysProp_map.begin(); physCurrentMap != mIdToPhysProp_map.end(); physCurrentMap++) {
		Phys_Properties* physProps = physCurrentMap->second;
		
		Topl_GeoNode* targetNode = mIdToGeo_map.at(physCurrentMap->first); // Gets the geometry node bound to the physics object
		
		if (physProps->actingForceCount > 0) {
			for (unsigned forceIndex = 0; forceIndex < physProps->actingForceCount; forceIndex++)
				physProps->acceleration += (*(physProps->forces + forceIndex) * magnifier) / physProps->mass;
		}
		// Velocity Integrator, 
		physProps->velocity += (physProps->acceleration * magnifier) * physElapseSecs; // Division converts elapsed time to seconds from milliseconds

		// Position integrator
		// targetNode->updatePos((physProps->velocity * physElapseSecs) + 0.5 * physProps->acceleration * pow(physElapseSecs, 2));
		targetNode->updatePos(physProps->velocity * physElapseSecs); // For now not factoring in acceleration

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