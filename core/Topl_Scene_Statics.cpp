#include "Topl_Scene.hpp"

// Scene Manager implementation code

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

void Topl_Scene::addGeometry(Geo_Actor* actor) {
	for(unsigned a = 0; a < _geoActors.size(); a ++)
		if(_geoActors[a]->getName() == actor->getName()){
			_geoActors[a] = actor; // overwrite geometry
			return;
		}
	_geoActors.push_back(actor); // add new geometry
}

void Topl_Scene::addGeometry(const std::string& name, Geo_Actor* actor) {
	actor->setName(name);
	addGeometry(actor);
}

#ifdef RASTERON_H

void Topl_Scene::addTexture(const std::string& name, const Rasteron_Image* image) {
	if (image->data == nullptr || image->height == 0 || image->width == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName())
			_actorTex_map.insert({ *actor, image });
}

const Rasteron_Image* Topl_Scene::getTexture(const std::string& name) const {
	for (std::map<Geo_Actor*, const Rasteron_Image*>::const_iterator m = _actorTex_map.cbegin(); m != _actorTex_map.cend(); m++)
		if (name == m->first->getName()) return m->second;
	return nullptr; // Error
}

#endif