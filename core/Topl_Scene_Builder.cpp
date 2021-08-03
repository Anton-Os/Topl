#include "Topl_Scene.hpp"

static void error_notFound(const std::string& objTypeStr, const std::string& name) {
	printf("Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

// Scene Manager implementation code

unsigned Geo_Component::mId_count = 0;

topl_geoComponent_cptr Topl_Scene::getGeoComponent(unsigned index) const {
	if(index >= mNamedGeos.size()) return nullptr; // Error
	else return mNamedGeos.at(index); 
}

topl_geoComponent_cptr Topl_Scene::getGeoComponent(const std::string& name) const {
	for(std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if((*currentGeo)->getName() == name) return *currentGeo;

	return nullptr; // Error
}

topl_linkedItems_cptr Topl_Scene::getLink(unsigned index) const {
	if(index > mLinkedItems.size()){
		puts("Index for linked items is out of range!");
		return nullptr;
	}

	return &mLinkedItems.at(index);
}

void Topl_Scene::addGeometry(const std::string& name, Geo_Component* geoComponent) {
	geoComponent->setName(name);
	mNamedGeos.push_back(geoComponent);
}

#ifdef RASTERON_H

void Topl_Scene::addTexture(const std::string& name, const Rasteron_Image* rastImage) {
	if (rastImage->data == nullptr || rastImage->height == 0 || rastImage->width == 0) return; // Error
	for (std::vector<Geo_Component*>::const_iterator currentGeo = mNamedGeos.cbegin(); currentGeo < mNamedGeos.cend(); currentGeo++)
		if (name == (*currentGeo)->getName()) {
			mGeoTex_map.insert({ *currentGeo, rastImage });
			return;
		}
}

const Rasteron_Image* Topl_Scene::getFirstTexture(const std::string& name) const {
	for (std::map<Geo_Component*, const Rasteron_Image*>::const_iterator currentMap = mGeoTex_map.cbegin(); currentMap != mGeoTex_map.cend(); currentMap++)
		if (name == currentMap->first->getName()) return currentMap->second;

	return nullptr; // Error
}

unsigned Topl_Scene::getTextures(unsigned index, const Rasteron_Image** images) const {
	// TODO: Rework and update this

	return 0;
}
#endif