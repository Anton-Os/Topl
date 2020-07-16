#include "Topl_SceneManager.hpp"

void Topl_GeoNode::updatePos(Eigen::Vector3f vec){
    mRelWorldPos += vec;
    for(unsigned c = 0; c < getChildCount(); c++)
        ((Topl_GeoNode*)getChild(c))->updatePos(vec); // Move all children as well
}


// More complex types

unsigned Topl_Node::mId_count = 0;

tpl_gEntity_cptr Topl_SceneManager::getGeoNode(unsigned index) const {
    if(mIdToGeo_map.find(index) == mIdToGeo_map.end()){
        puts("Index provided cannot be found");
        return nullptr;
    }

    return mIdToGeo_map.at(index);
}

tpl_gEntity_cptr Topl_SceneManager::getGeoNode(const std::string& name) const {
    if(mNameToId_map.find(name) == mNameToId_map.end()){
        puts("Name provided cannot be found");
        return nullptr;
    } 

    unsigned gIndex = mNameToId_map.at(name);
    return mIdToGeo_map.at(gIndex);
}

void Topl_SceneManager::addGeometry(const std::string& name, Topl_GeoNode* geoNode){
    if(mNameToId_map.find(name) != mNameToId_map.end()){
        puts("Overriding geometry object:");
        puts(name.c_str());
    }

    geoNode->setName(name);


    std::string targetName = (geoNode->getParent() == nullptr) ? name : "_parent_child"; // TODO: Fix the association

    mNameToId_map.insert({ targetName, ((Topl_Node*)geoNode)->getId() });
    mIdToGeo_map.insert({ ((Topl_Node*)geoNode)->getId(), geoNode });
}

void Topl_SceneManager::addForce(const std::string& name, const Eigen::Vector3f& vec){
    if(mNameToId_map.find(name) == mNameToId_map.end()){
        puts("Could not find geometry object:");
        puts(name.c_str());
        return;
    }

    Topl_GeoNode* targetNode = mIdToGeo_map.at(mNameToId_map.at(name));
    vec3f_cptr targetPos = targetNode->getPos();
    // float targetWeight = targetNode->getWeight();
    // PHYSICS HANDLING CODE!
    // MAKE A PROPER INTEGRATOR
    // float accel, velocity, pos;
    targetNode->updatePos(vec);
}

#ifdef RASTERON_H
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