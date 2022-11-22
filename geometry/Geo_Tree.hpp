#ifndef GEO_TREE_H

#include <initializer_list>
#include <string>

#include "Geometry.hpp"

#include "Topl_Scene.hpp"

typedef std::pair<std::string, Geo_Actor*> geoName_pair;
typedef std::pair<Vec3f, Vec3f> orientation_pair;

#define NO_ORIENTATION std::make_pair(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f));

// Tree

class Geo_Tree {
public:
	Geo_Tree(const std::string& prefix) { _prefix = prefix;  } // Empty constructor
    Geo_Tree(const std::string& prefix, std::initializer_list<Geo_RenderObj*> renderObjs); // Fixed items constructor
    Geo_Tree(const std::string& prefix, const Geo_Actor* actor, unsigned count); // Duplicate items constructor

	~Geo_Tree();

    std::string getPrefix(){ return _prefix + "_"; }
    Vec3f getOrigin();
    void move(Vec3f vec){ 
        for(unsigned g = 0; g < _actorCount; g++) (*(_actorData + g))->updatePos(vec);
    }
	void rotate(Vec3f angles); // rotation of actors around origin
    void rotateAll(Vec3f angles){ // piecewise rotation of all actors
        for(unsigned g = 0; g < _actorCount; g++) (*(_actorData + g))->updateRot(angles);
    }

    virtual void configure(Topl_Scene* scene) = 0; // function to pass data into  scene
protected:
	unsigned getActorCount() const { return _actorCount; }
    Geo_Actor* getNextActor();
private:
    std::string _prefix;
	unsigned _actorCount = 0;
    unsigned _actorOffset = 0;
	Geo_Actor** _actorData = nullptr; // actor data is stored here and retrieved sequentially by derived class
};

// Dynamic Set

struct Geo_DynamicSet {
	Geo_DynamicSet(unsigned setCount) { physActors.resize(setCount); }

	std::vector<Phys_Actor> physActors; // physics actors
	std::vector<Phys_Connector> links; // links
};

#define GEO_TREE_H
#endif