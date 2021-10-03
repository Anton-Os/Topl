#include "Geo_Troupe.hpp"

Geo_Troupe::Geo_Troupe(const std::string& prefix, Topl_Scene* scene, std::initializer_list<Geo_RenderObj*> renderObjs){
    _prefix = prefix;
    _geoData = (Geo_Actor**)malloc(renderObjs.size() * sizeof(Geo_Actor*));
    for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
        *(_geoData + _geoCount) = new Geo_Actor(*(currentRenderObj));
        _geoCount++;
    }
}

Geo_Troupe::Geo_Troupe(const std::string& prefix, Topl_Scene* scene, const Geo_Actor* geoc, unsigned count){
    _prefix = prefix;
    _geoCount = count;
    _geoData = (Geo_Actor**)malloc(count * sizeof(Geo_Actor));

    for(unsigned g = 0; g < count; g++) *(_geoData + g) = new Geo_Actor(*geoc);
}

Geo_Troupe::~Geo_Troupe(){
    if(_geoData != nullptr) { 
        for (unsigned g = 0; g < _geoCount; g++)
            delete *(_geoData + g);
        free(_geoData);
    }
}

void Geo_Troupe::fillScene(Topl_Scene* scene){
    fill(scene);
    // Code that fills in scene
    if(_namedGeos.size() != 0 && _geoData != nullptr)
        for(std::vector<geoName_pair>::iterator currentGeo = _namedGeos.begin();currentGeo < _namedGeos.end(); currentGeo++)
            scene->addGeometry(currentGeo->first, currentGeo->second);
}

Geo_Actor* Geo_Troupe::getNextGeo(){
    if(_currentGeoOffset <= _geoCount){
        _currentGeoOffset++;   
        return *(_geoData + _currentGeoOffset - 1); // to increment offset above in one line
    } else return nullptr;
}