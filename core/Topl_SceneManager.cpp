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

void Topl_SceneManager::addForce(const std::string& name, const Eigen::Vector3f& vec) {
	if (mNameToId_map.find(name) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name);

	Topl_GeoNode* targetNode = mIdToGeo_map.at(mNameToId_map.at(name));
	vec3f_cptr targetPos = targetNode->getPos();

	if (mIdToPhysProp_map.find(mNameToId_map.at(name)) == mIdToPhysProp_map.end())
		return print_ObjNotFound("physics", name);

	// pProp->acceleration = vec / pProp->mass;
	// Integrate all the way to position
	// Increment acting force count and resolve acting forces on object

	targetNode->updatePos(vec);
}

void Topl_SceneManager::addPhysics(const std::string& name, Phys_Properties* pProp) {
	if (mNameToId_map.find(name) == mNameToId_map.end())
		return print_ObjNotFound("geometry", name);

	mIdToPhysProp_map.insert({ mNameToId_map.at(name), pProp });
}

void Topl_SceneManager::resolvePhysics() {
	for (std::map<unsigned, Phys_Properties*>::iterator physCurrent = mIdToPhysProp_map.begin(); physCurrent != mIdToPhysProp_map.end(); physCurrent++) {
		// Step through each, locate associated Topl_Geo node and update its position and orientation
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