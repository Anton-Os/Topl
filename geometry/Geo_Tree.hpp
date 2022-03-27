#ifndef GEO_TREE_H

#include <initializer_list>
#include <string>

#include "Geometry.hpp"
// #include "Timer.hpp"

#include "Topl_Scene.hpp"

// Geometry wrapper class that can manage its states

typedef std::pair<std::string, Geo_Actor*> geoName_pair;
typedef std::pair<Vec3f, Vec2f> orientation_pair;

#define NO_ORIENTATION std::make_pair(Vec3f(0.0f, 0.0f, 0.0f), Vec2f(0.0f, 0.0f));


class Geo_Tree {
public:
    // No items constructor
    Geo_Tree(const std::string& prefix){}
    // Fixed items constructor
    Geo_Tree(const std::string& prefix, std::initializer_list<Geo_RenderObj*> renderObjs);
    // Duplicate items constructor
    Geo_Tree(const std::string& prefix, const Geo_Actor* actor, unsigned count);

	~Geo_Tree();

    std::string getPrefix(){ return _prefix + "_"; }
    Vec3f getOrigin();
    void move(Vec3f vec){ 
        for(unsigned g = 0; g < _actorCount; g++) (*(_actorData + g))->updatePos(vec); 
    }
	void rotate(Vec2f angles); // rotation of actors around origin
    void rotateAll(Vec2f angles){ // piecewise rotation of all actors
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

struct Geo_DynamicSet { // A container that holds objects used in physics
    Geo_DynamicSet(unsigned setCount){ phys.resize(setCount); }

    std::vector<Phys_Actor> phys; // container for physics actors
    std::vector<Phys_Connector> links; // contatiner for links
};

#define GEO_TREE_H
#endif