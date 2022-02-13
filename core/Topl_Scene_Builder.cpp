#include "Topl_Scene.hpp"

static void error_notFound(const std::string& objTypeStr, const std::string& name) {
	printf("Could not find %s object: \n", objTypeStr.c_str());
	puts(name.c_str());
	putchar('\n');
}

// Scene Manager implementation code

unsigned Geo_Actor::_id_count = 0;

actor_cptr Topl_Scene::getGeoActor(unsigned index) const {
	if(index >= _geoActors.size()) return nullptr; // Error
	else return _geoActors.at(index); 
}

actor_cptr Topl_Scene::getGeoActor(const std::string& name) const {
	for(std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if((*actor)->getName() == name) return *actor;

	return nullptr; // Error
}

lightSource_cptr Topl_Scene::getLightSource(unsigned index) const {
	if(index > _lightSrc.size()) return nullptr;
	else return _lightSrc.at(index);
}

linkedItems_cptr Topl_Scene::getLink(unsigned index) const {
	if(index > _linkedItems.size()){
		puts("Index for linked items is out of range!");
		return nullptr;
	}
	return &_linkedItems.at(index);
}

void Topl_Scene::addGeometry(const std::string& name, Geo_Actor* geo) {
	geo->setName(name);
	_geoActors.push_back(geo);
}

#ifdef RASTERON_H

/* void Topl_Rasteron_AnimWrap::addFrame(const Rasteron_Image *const refImg){
	if(frameIndex >= MAX_TEXTURES_PER_ACTOR){
		frameIndex = 0;
		isOverride = true;
	}
	addFrameData(animation, refImg, frameIndex);
	frameIndex++;
}

Rasteron_Image* Topl_Rasteron_AnimWrap::getFrameNamed(std::string name){
	return nullptr; // TODO: search through and retrieve animation contents here
} */

void Topl_Scene::addTexture(const std::string& name, const Rasteron_Image* image) {
	if (image->data == nullptr || image->height == 0 || image->width == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_actorTexture_map.insert({ *actor, image });
			return;
		}
}

const Rasteron_Image* Topl_Scene::getFirstTexture(const std::string& name) const {
	for (std::map<Geo_Actor*, const Rasteron_Image*>::const_iterator currentMap = _actorTexture_map.cbegin(); currentMap != _actorTexture_map.cend(); currentMap++)
		if (name == currentMap->first->getName()) return currentMap->second;

	return nullptr; // Error
}

#endif