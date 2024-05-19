#include "Topl_Scene.hpp"

void Topl_Scene::addGeometry(Geo_Actor* actor) {
	for(unsigned a = 0; a < _geoActors.size(); a++)
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

Geo_Actor* Topl_Scene::getPickActor(unsigned color){
	for(std::vector<Geo_Actor*>::iterator actor = _geoActors.begin(); actor < _geoActors.end(); actor++)
		if(((*actor)->getId() & 0x000000FF) == (color & 0x000000FF)) return *actor;
	return nullptr; // no actor found
}

actor_cptr Topl_Scene::getGeoActor(unsigned index) const {
	if (index < _geoActors.size()) return _geoActors.at(index);
	else return nullptr;
}

actor_cptr Topl_Scene::getGeoActor(const std::string& name) const {
	for(std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if((*actor)->getName() == name) return *actor;
	return nullptr; // Error
}

/* light_cptr Topl_Scene::getLight(unsigned index) const {
	if(index > _lights.size()) return nullptr;
	else return _lights.at(index);
} */

void Topl_Scene::removeActor(const std::string& name){
	Geo_Actor* actor = nullptr;
	for(std::vector<Geo_Actor*>::const_iterator a = _geoActors.cbegin(); a < _geoActors.cend(); a++)
		if((*a)->getName() == name) actor = *a;

	_pickerCallbackMap.erase(actor);
#ifdef TOPL_ENABLE_PHYSICS
	_physicsMap.erase(actor);
#endif
#ifdef RASTERON_H
	_textureMap.erase(actor);
	_arrayTexMap.erase(actor);
	_volumeTexMap.erase(actor);
#endif
	std::remove(_geoActors.begin(), _geoActors.end(), actor);
}

#ifdef RASTERON_H

void Topl_Scene::addTexture(const std::string& name, const Img_Base* image) {
	// if (image->data == nullptr || image->height == 0 || image->width == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_textureMap.erase(*actor);
			_textureMap.insert({ *actor, image });
		}
}

const Img_Base* Topl_Scene::getTexture(const std::string& name) const {
	if(name == "1" || name == "2" || name == "3" || name == "4" || name == "5" || name == "6" || name == "7")
		return &_textures[(char)name[0] - '0' - 1]; // unbound texture
	else if(_textureMap.empty()) return nullptr; // no textures available
	auto texture_it = std::find_if(
		_textureMap.begin(), _textureMap.end(),
		[name](const std::pair<Geo_Actor*, const Img_Base*>& p){ return p.first->getName() == name; }
	);
	return (texture_it != _textureMap.end())? texture_it->second : nullptr;
}

void Topl_Scene::addArrayTex(const std::string& name, const Img_Array* arrayTex) {
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_arrayTexMap.erase(*actor);
			_arrayTexMap.insert({ *actor, arrayTex });
		}
}

const Img_Array* Topl_Scene::getArrayTex(const std::string& name) const {
	if(_arrayTexMap.empty()) return nullptr; // no textures available
	auto arrayTex_it = std::find_if(
		_arrayTexMap.begin(), _arrayTexMap.end(),
		[name](const std::pair<Geo_Actor*, const Img_Array*>& p){ return p.first->getName() == name; }
	);
	return (arrayTex_it != _arrayTexMap.end())? arrayTex_it->second : nullptr;
}

void Topl_Scene::addVolumeTex(const std::string& name, const Img_Volume* volumeTexTex) {
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_volumeTexMap.erase(*actor);
			_volumeTexMap.insert({ *actor, volumeTexTex });
		}
}

const Img_Volume* Topl_Scene::getVolumeTex(const std::string& name) const {
	if(_volumeTexMap.empty()) return nullptr; // no textures available
	auto volumeTex_it = std::find_if(
		_volumeTexMap.begin(), _volumeTexMap.end(),
		[name](const std::pair<Geo_Actor*, const Img_Volume*>& p){ return p.first->getName() == name; }
	);
	return (volumeTex_it != _volumeTexMap.end())? volumeTex_it->second : nullptr;
}

#endif