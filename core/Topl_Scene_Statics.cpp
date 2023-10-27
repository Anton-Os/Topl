#include "Topl_Scene.hpp"

// Scene Statics

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

void Topl_Scene::addPickerCallback(const std::string& name, pickerCallback callback){
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if(name == (*actor)->getName()) _pickerCallbackMap.insert({ *actor, callback }); 
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

light_cptr Topl_Scene::getLight(unsigned index) const {
	if(index > _lights.size()) return nullptr;
	else return _lights.at(index);
}

pickerCallback Topl_Scene::getPickerCallback(unsigned color){
	for(std::map<Geo_Actor*, pickerCallback>::const_iterator p = _pickerCallbackMap.cbegin(); p != _pickerCallbackMap.cend(); p++)
		if(color == p->first->getId()) return p->second;
	return nullptr; // no callback
}

#ifdef RASTERON_H

void Topl_Scene::addTexture(const std::string& name, const Rasteron_Image* image) {
	if (image->data == nullptr || image->height == 0 || image->width == 0) return; // Error
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_textureMap.erase(*actor);
			_textureMap.insert({ *actor, image });
		}
}

const Rasteron_Image* Topl_Scene::getTexture(const std::string& name) const {
	for (std::map<Geo_Actor*, const Rasteron_Image*>::const_iterator t = _textureMap.cbegin(); t != _textureMap.cend(); t++)
		if (name == t->first->getName()) return t->second;
	return nullptr; // Error
}

void Topl_Scene::addMaterialTex(const std::string& name, const Img_Material* material) {
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_materialMap.erase(*actor);
			_materialMap.insert({ *actor, material });
		}
}

const Img_Material* Topl_Scene::getMaterialTex(const std::string& name) const {
	for (std::map<Geo_Actor*, const Img_Material*>::const_iterator m = _materialMap.cbegin(); m != _materialMap.cend(); m++)
		if (name == m->first->getName()) return m->second;
	return nullptr; // Error
}

void Topl_Scene::addVolumeTex(const std::string& name, const Img_Volume* volumeTex) {
	for (std::vector<Geo_Actor*>::const_iterator actor = _geoActors.cbegin(); actor < _geoActors.cend(); actor++)
		if (name == (*actor)->getName()) {
			_volumeMap.erase(*actor);
			_volumeMap.insert({ *actor, volumeTex });
		}
}

const Img_Volume* Topl_Scene::getVolumeTex(const std::string& name) const {
	for (std::map<Geo_Actor*, const Img_Volume*>::const_iterator v = _volumeMap.cbegin(); v != _volumeMap.cend(); v++)
		if (name == v->first->getName()) return v->second;
	return nullptr; // Error
}

#endif