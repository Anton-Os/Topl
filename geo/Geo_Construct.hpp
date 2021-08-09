#ifndef GEO_CONSTRUCT_H

#include <initializer_list>
#include <string>

#include "Geometry.hpp"
#include "Timer.hpp"

#include "Topl_Scene.hpp"

// Geometry wrapper class that can manage its states

#define FIRST_UPDATE_NUM 0

typedef std::pair<std::string, Geo_Component*> geoName_pair;

class Geo_Construct {
public:
    // Fixed items constructor
    Geo_Construct(const std::string& prefix, Topl_Scene* scene, std::initializer_list<Geo_RenderObj*> renderObjs) {
		_prefix = prefix;
        _geoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(_geoData + _geoCount) = new Geo_Component(*(currentRenderObj));
            _geoCount++;
        }
	}
    // Duplicate items constructor
    Geo_Construct(const std::string& prefix, Topl_Scene* scene, const Geo_Component* geoc, unsigned count) {
		_prefix = prefix;
        _geoCount = count;
        _geoData = (Geo_Component**)malloc(count * sizeof(Geo_Component));

        for(unsigned g = 0; g < count; g++) *(_geoData + g) = new Geo_Component(*geoc);
	}

	~Geo_Construct() { 
        if(_geoData != nullptr) { 
            for (unsigned g = 0; g < _geoCount; g++)
                delete *(_geoData + g);
            free(_geoData);
        }
    }

    // Should be called in the derived class constructor body
    void fillScene(Topl_Scene* scene){
        fill(scene);

        // Code that fills in scene
        if(_namedGeos.size() != 0 && _geoData != nullptr) {
            for(std::vector<geoName_pair>::iterator currentGeo = _namedGeos.begin();
                currentGeo < _namedGeos.end(); currentGeo++)
                scene->addGeometry(currentGeo->first, currentGeo->second);
        }
    }

    std::string getPrefix(){ return _prefix + "_"; }
	virtual void updateScene(Topl_Scene* scene) = 0;
    void move(Topl_Scene* scene, Eigen::Vector3f vec){
        for(unsigned g = 0; g < _geoCount; g++) scene->addForce((*(_geoData + g))->getName(), vec);
    }
    void rotate(Topl_Scene* scene, Eigen::Vector2f angles){
        for(unsigned g = 0; g < _geoCount; g++) (*(_geoData + g))->updateRot(angles);
    }
    /* virtual void move(Topl_Scene* scene, Eigen::Vector3f vec) = 0;
    virtual void rotate(Topl_Scene* scene, Eigen::Vector3f) = 0; */

protected:
	unsigned getGeoCount() const { return _geoCount; }
    virtual void fill(Topl_Scene* scene) = 0; // job is to fill the _namedGeos structure
    Geo_Component* getNextGeo(){
        if(_currentGeoOffset <= _geoCount){
            _currentGeoOffset++;   
            return *(_geoData + _currentGeoOffset - 1); // to increment offset above in one line
        } else return nullptr;
    }

    unsigned _updateCount = FIRST_UPDATE_NUM; // probably needs to be private and a getter method
	std::vector<geoName_pair> _namedGeos;
private:
    std::string _prefix;
	unsigned _geoCount = 0;
    unsigned _currentGeoOffset = 0;
	Geo_Component** _geoData = nullptr;
}; // needs work! could use more abstraction

struct Geo_DynamicSet { // A container for multiple dynamic objects
    Geo_DynamicSet(unsigned setCount){
        phys.resize(setCount);
    }

    std::vector<Phys_Properties> phys;
    std::vector<Phys_Connector> links;
};

#define GEO_CONSTRUCT_H
#endif