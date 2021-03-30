#include "Topl_SceneManager.hpp"

static void print_ObjNotFound(const std::string& objTypeStr, const std::string& name) {
	printf("Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

// Scene Manager implementation code

unsigned Geo_Component::mId_count = 0;

topl_geoComponent_cptr Topl_SceneManager::getGeoComponent(unsigned index) const {
	if(index >= mNamedGeos.size()) return nullptr; // Error
	else return mNamedGeos.at(index); 
}

topl_geoComponent_cptr Topl_SceneManager::getGeoComponent(const std::string& name) const {
	for(std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if((*currentGeo)->getName() == name) return *currentGeo;

	return nullptr; // Error
}

topl_linkedItems_cptr Topl_SceneManager::getLink(unsigned index) const {
	if(index > mLinkedItems.size()){
		puts("Index for linked items is out of range!");
		return nullptr;
	}

	return &mLinkedItems.at(index);
}

void Topl_SceneManager::addGeometry(const std::string& name, Geo_Component* geoComponent) {
	geoComponent->setName(name);

	mNamedGeos.push_back(geoComponent);
}

#ifdef RASTERON_H

void Topl_SceneManager::addTexture(const std::string& name, const Rasteron_Image* rstnImage) {
	if (rstnImage->data == nullptr || rstnImage->height == 0 || rstnImage->width == 0) return; // Error
	for (std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if (name == (*currentGeo)->getName()) {
			mGeoTex_map.insert({ *currentGeo, rstnImage });
			return;
		}
}

const Rasteron_Image* Topl_SceneManager::getFirstTexture(const std::string& name) const {
	for (std::map<Geo_Component*, const Rasteron_Image*>::const_iterator currentMap = mGeoTex_map.cbegin(); currentMap != mGeoTex_map.cend(); currentMap++)
		if (name == currentMap->first->getName()) return currentMap->second;

	return nullptr; // Error
}

unsigned Topl_SceneManager::getTextures(unsigned index, const Rasteron_Image** images) const {
	// TODO: Rework and update this

	return 0;
}
#endif