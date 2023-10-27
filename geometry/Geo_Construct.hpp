#ifndef GEO_CONSTRUCT_H

#include <initializer_list>
#include <string>

#include "Geo_Mesh.hpp"

#include "Topl_Scene.hpp"

typedef std::pair<std::string, Geo_Actor*> geoName_pair; // MOVE THIS!!!
typedef std::pair<Vec3f, Vec3f> orientation_pair; // MOVE THIS!!!

#define NO_ORIENTATION std::make_pair(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f)); // MOVE THIS!!!

// Tree

class Geo_Construct {
public:
	Geo_Construct(const std::string& prefix) { _prefix = prefix; } // Empty constructor
    // Geo_Construct(const std::string& prefix, std::initializer_list<Geo_Mesh*> meshes) // Fixed items constructor
    Geo_Construct(const std::string& prefix, const Geo_Actor* actor, unsigned count){ // Duplicate items constructor
        _prefix = prefix;
        _geoActors.resize(count);
        for(unsigned g = 0; g < count; g++) _geoActors[g] = Geo_Actor(*actor);
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
    void move(Vec3f vec){ 
        for(unsigned g = 0; g < _geoActors.size(); g++) _geoActors[g].updatePos(vec);
    }
    void rotateAll(Vec3f angles){ // piecewise rotation of all actors
        for(unsigned g = 0; g < _geoActors.size(); g++) _geoActors[g].updateRot(angles);
    }
	void rotate(Vec3f angles){ // full rotation around origin
        rotateAll(angles);

        Vec3f origin = getOrigin();
        for(unsigned a = 0; a < _geoActors.size(); a++){
            Geo_Actor* actor =  &_geoActors[a];
            Vec3f actor_pos = *actor->getPos() - origin;
            
            float x = (actor_pos[0] * cos(angles[0])) - (actor_pos[1] * sin(angles[0]));
            float y = (actor_pos[0] * sin(angles[0])) + (actor_pos[1] * cos(angles[0]));
            float z = actor_pos[2];
            actor_pos = Vec3f({ x, y, z });
            actor->setPos(origin + actor_pos);
        }
    }

    virtual void configure(Topl_Scene* scene) = 0; // function to pass data into  scene
protected:
    Vec3f _origin = Vec3f({ 0.0f, 0.0f, 0.0f });

    std::string _prefix;
    std::vector<Geo_Actor> _geoActors;
    std::vector<Phys_Actor> _physActors; // physics actors
	std::vector<Phys_Connector> _links; // links
};

#define GEO_CONSTRUCT_H
#endif