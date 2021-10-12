#ifndef GEO_CONSTRUCT_H

#include <initializer_list>
#include <string>

#include "Geometry.hpp"
#include "Timer.hpp"

#include "Topl_Scene.hpp"

// Geometry wrapper class that can manage its states

#define FIRST_UPDATE_NUM 0

typedef std::pair<std::string, Geo_Actor*> geoName_pair;

class Geo_Tree {
public:
    // Fixed items constructor
    Geo_Tree(const std::string& prefix, Topl_Scene* scene, std::initializer_list<Geo_RenderObj*> renderObjs);
    // Duplicate items constructor
    Geo_Tree(const std::string& prefix, Topl_Scene* scene, const Geo_Actor* geoc, unsigned count);

	~Geo_Tree();

    void move(Topl_Scene* scene, Eigen::Vector3f vec){ 
        for(unsigned g = 0; g < _actorCount; g++) (*(_actorData + g))->updatePos(vec); 
    }
    void rotate(Topl_Scene* scene, Eigen::Vector2f angles){
        for(unsigned g = 0; g < _actorCount; g++) (*(_actorData + g))->updateRot(angles);
    }
    std::string getPrefix(){ return _prefix + "_"; }
protected:
    virtual void fill(Topl_Scene* scene) = 0; 

	unsigned getActorCount() const { return _actorCount; }
    Geo_Actor* getNextActor();
private:
    std::string _prefix;
	unsigned _actorCount = 0;
    unsigned _actorOffset = 0;
	Geo_Actor** _actorData = nullptr; // actor data is stored here and retrieved sequentially by derived class
}; // needs work! could use more abstraction

struct Geo_DynamicSet { // A container that holds objects used in physics
    Geo_DynamicSet(unsigned setCount){ phys.resize(setCount); }

    std::vector<Phys_Actor> phys;
    std::vector<Phys_Connector> links;
};

#define GEO_CONSTRUCT_H
#endif