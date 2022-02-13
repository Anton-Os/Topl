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

light_cptr Topl_Scene::getLight(unsigned index) const {
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

void Topl_MultiTex::addFrame(const Rasteron_Image *const refImg){
	if(frameIndex >= MAX_TEXTURES_PER_ACTOR){
		frameIndex = 0;
		isOverride = true;
	}
	// addFrameData(animation, refImg, frameIndex);
	frameIndex++;
}

Rasteron_Image* Topl_MultiTex::getFrameNamed(const std::string& name) const{
	return nullptr; // TODO: search through and retrieve animation contents here
}

void Topl_Scene::addTexture(const std::string& name, const Rasteron_Image* image) {
	if (image->data == nullptr || image->height == 0 || image->width == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName())
			_actorTex_map.insert({ *actor, image });
}

void Topl_Scene::addMultiTex(const std::string& name, const Topl_MultiTex* multiTex) {
	if(multiTex->frameIndex == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName())
			_actorMultiTex_map.insert({ *actor, multiTex });
}

const Rasteron_Image* Topl_Scene::getTexture(const std::string& name) const {
	for (std::map<Geo_Actor*, const Rasteron_Image*>::const_iterator m = _actorTex_map.cbegin(); m != _actorTex_map.cend(); m++)
		if (name == m->first->getName()) return m->second;
	return nullptr; // Error
}

const Rasteron_Image* Topl_Scene::getTexture(const std::string& name, unsigned frameIndex) const {
	for (std::map<Geo_Actor*, const Topl_MultiTex*>::const_iterator m = _actorMultiTex_map.cbegin(); m != _actorMultiTex_map.cend(); m++)
		if (name == m->first->getName()) 
			return m->second->getFrameNamed(""); // TODO: search for proper matchinf rame name
	return nullptr; // Error
}

#endif