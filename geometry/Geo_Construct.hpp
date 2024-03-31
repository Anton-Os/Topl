#ifndef GEO_CONSTRUCT_H

#include <initializer_list>
#include <string>

#include "Geo_Mesh.hpp"

#include "Topl_Scene.hpp"

// Tree

class Geo_Construct {
public:
	Geo_Construct(const std::string& prefix) { _prefix = prefix; } // Empty constructor
    // Geo_Construct(const std::string& prefix, std::initializer_list<Geo_Mesh*> meshes) // Fixed items constructor
    Geo_Construct(const std::string& prefix, const Geo_Mesh* mesh, unsigned count){ // Duplicate items constructor
        _prefix = prefix;
        if(_geoActors.empty()) _geoActors.resize(count);
#ifdef TOPL_ENABLE_PHYSICS    
        if(_physActors.empty()) _physActors.resize(count);
#endif
        for(unsigned g = 0; g < count; g++) _geoActors[g] = Geo_Actor(mesh);
    }

    std::string getPrefix(){ return _prefix + "_"; }
    Vec3f getOrigin(){
        _origin = Vec3f({ 0.0f, 0.0f, 0.0f });
        for(unsigned a = 0; a < _geoActors.size(); a++){
            Geo_Actor* actor =  &_geoActors[a];
            _origin = _origin + *actor->getPos();
        }
        _origin = Vec3f({ _origin[0] / _geoActors.size(), _origin[1] / _geoActors.size(), _origin[2] / _geoActors.size() });
        return _origin;
    }
    Geo_Actor* getGeoActor(unsigned short a){ return &_geoActors[a]; }
    void shift(Vec3f vec){ 
        for(unsigned g = 0; g < _geoActors.size(); g++) _geoActors[g].updatePos(vec);
    }
    void toggleShow(){
        for(unsigned g = 0; g < _geoActors.size(); g++) _geoActors[g].isShown = !_geoActors[g].isShown;
    }
    void rotateAll(Vec3f angles){ // piecewise rotation of all actors
        for(unsigned g = 0; g < _geoActors.size(); g++) _geoActors[g].updateRot(angles);
    }
	void rotate(Vec3f angles){ // full rotation around origin
        rotateAll(angles);

        for(unsigned a = 0; a < _geoActors.size(); a++){
            Geo_Actor* actor =  &_geoActors[a];
            Vec3f actor_pos = *actor->getPos() - getOrigin();
            
            float x = (actor_pos[0] * cos(angles[0])) - (actor_pos[1] * sin(angles[0]));
            float y = (actor_pos[0] * sin(angles[0])) + (actor_pos[1] * cos(angles[0]));
            float z = actor_pos[2];
            actor_pos = Vec3f({ x, y, z });
            actor->setPos(getOrigin() + actor_pos);
        }
    }
    void scale(Vec3f scaleVec){
        for(unsigned g = 0; g < _geoActors.size(); g++) _geoActors[g].setSize(scaleVec);
    }

    virtual void configure(Topl_Scene* scene) = 0; // function to pass data into  scene
protected:
    Vec3f _origin = Vec3f({ 0.0f, 0.0f, 0.0f });

    std::string _prefix;
    std::vector<Geo_Actor> _geoActors;
#ifdef TOPL_ENABLE_PHYSICS
    std::vector<Phys_Actor> _physActors; // physics actors
	std::vector<Phys_Connector> _links; // physics links
#endif
};

#define GEO_CONSTRUCT_H
#endif