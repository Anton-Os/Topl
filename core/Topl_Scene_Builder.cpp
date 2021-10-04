#include "Topl_Scene.hpp"

static void error_notFound(const std::string& objTypeStr, const std::string& name) {
	printf("Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

// Scene Manager implementation code

unsigned Geo_Actor::_id_count = 0;

topl_geo_cptr Topl_Scene::getGeoActor(unsigned index) const {
	if(index >= _namedGeos.size()) return nullptr; // Error
	else return _namedGeos.at(index); 
}

topl_geo_cptr Topl_Scene::getGeoActor(const std::string& name) const {
	for(std::vector<Geo_Actor*>::const_iterator currentGeo = _namedGeos.cbegin(); currentGeo < _namedGeos.cend(); currentGeo++)
		if((*currentGeo)->getName() == name) return *currentGeo;

	return nullptr; // Error
}

topl_linkedItems_cptr Topl_Scene::getLink(unsigned index) const {
	if(index > _linkedItems.size()){
		puts("Index for linked items is out of range!");
		return nullptr;
	}
	return &_linkedItems.at(index);
}

void Topl_Scene::addGeometry(const std::string& name, Geo_Actor* geo) {
	geo->setName(name);
	_namedGeos.push_back(geo);
}

#ifdef RASTERON_H

void Topl_Scene::addTexture(const std::string& name, const Rasteron_Image* rastImage) {
	if (rastImage->data == nullptr || rastImage->height == 0 || rastImage->width == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator currentGeo = _namedGeos.cbegin(); currentGeo < _namedGeos.cend(); currentGeo++)
		if (name == (*currentGeo)->getName()) {
			_geoTex_map.insert({ *currentGeo, rastImage });
			return;
		}
}

const Rasteron_Image* Topl_Scene::getFirstTexture(const std::string& name) const {
	for (std::map<Geo_Actor*, const Rasteron_Image*>::const_iterator currentMap = _geoTex_map.cbegin(); currentMap != _geoTex_map.cend(); currentMap++)
		if (name == currentMap->first->getName()) return currentMap->second;

	return nullptr; // Error
}

unsigned Topl_Scene::getTextures(unsigned index, const Rasteron_Image** images) const {
	// TODO: Rework and update this

	return 0;
}
#endif