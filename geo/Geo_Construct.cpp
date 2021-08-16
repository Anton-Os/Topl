#include "Geo_Construct.hpp"

Geo_Construct::Geo_Construct(const std::string& prefix, Topl_Scene* scene, std::initializer_list<Geo_RenderObj*> renderObjs){
    _prefix = prefix;
    _geoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
    for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
        *(_geoData + _geoCount) = new Geo_Component(*(currentRenderObj));
        _geoCount++;
    }
}

Geo_Construct::Geo_Construct(const std::string& prefix, Topl_Scene* scene, const Geo_Component* geoc, unsigned count){
    _prefix = prefix;
    _geoCount = count;
    _geoData = (Geo_Component**)malloc(count * sizeof(Geo_Component));

    for(unsigned g = 0; g < count; g++) *(_geoData + g) = new Geo_Component(*geoc);
}

Geo_Construct::~Geo_Construct(){
    if(_geoData != nullptr) { 
        for (unsigned g = 0; g < _geoCount; g++)
            delete *(_geoData + g);
        free(_geoData);
    }
}

void Geo_Construct::fillScene(Topl_Scene* scene){
    fill(scene);
    // Code that fills in scene
    if(_namedGeos.size() != 0 && _geoData != nullptr)
        for(std::vector<geoName_pair>::iterator currentGeo = _namedGeos.begin();currentGeo < _namedGeos.end(); currentGeo++)
            scene->addGeometry(currentGeo->first, currentGeo->second);
}

Geo_Component* Geo_Construct::getNextGeo(){
    if(_currentGeoOffset <= _geoCount){
        _currentGeoOffset++;   
        return *(_geoData + _currentGeoOffset - 1); // to increment offset above in one line
    } else return nullptr;
}