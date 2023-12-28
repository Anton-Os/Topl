#ifndef GEO_PUPPET_H

#include "Geo_Construct.hpp"

#define PUPPET_PARTS 6

enum PUPPET_Part {
    PUPPET_Head = 0,
	PUPPET_LeftArm = 1,
	PUPPET_RightArm = 2,
	PUPPET_Torso = 3,
	PUPPET_LeftLeg = 4,
	PUPPET_RightLeg = 5
};

typedef const Geo_Actor* puppetBlocks[PUPPET_PARTS];

struct Geo_Puppet : public Geo_Construct {
    Geo_Puppet(const std::string& prefix) : Geo_Construct(prefix){} // default constructor
    Geo_Puppet(const std::string& prefix, puppetBlocks parts) : Geo_Construct(prefix){
        _geoActors.resize(PUPPET_PARTS);
        for(unsigned p = 0; p < PUPPET_PARTS; p++) _geoActors[p] = *parts[p];
    }
    Geo_Puppet(const std::string& prefix, puppetBlocks parts, Topl_Scene* scene) : Geo_Construct(prefix){
        _geoActors.resize(PUPPET_PARTS);
        for(unsigned p = 0; p < PUPPET_PARTS; p++) _geoActors[p] = *parts[p];
        configure(scene);
    }

    void configure(Topl_Scene* scene) override{
        // TODO: Add body here
    }
};

#ifdef RASTERON_H

typedef const std::string puppetSpritePaths[PUPPET_PARTS]; // list of paths for loading sprites

class Geo_Puppet2D : public Geo_Puppet {
public:
    Geo_Puppet2D(const std::string& prefix) : Geo_Puppet(prefix){}
    Geo_Puppet2D(const std::string& prefix, puppetSpritePaths paths) : Geo_Puppet(prefix){} // TODO: Generate actors with sprite data
    // Geo_Puppet2D(const std::string& prefix, const std::string& fileImgs[PUPPET_PARTS], Topl_Scene* scene) : Geo_Puppet(prefix, &actors, scene){}

protected:
    Geo_Actor* actors[PUPPET_PARTS];
    Rasteron_Sprite* sprites[PUPPET_PARTS];
};

#endif
#ifdef TOPL_ENABLE_MODELS

#include "meshes/Geo_Node.hpp"

typedef const std::string puppetModelPaths[PUPPET_PARTS]; // list of paths for loading models

class Geo_Puppet3D : public Geo_Puppet {
public:
    Geo_Puppet3D(const std::string& prefix) : Geo_Puppet(prefix){}
    Geo_Puppet3D(const std::string& prefix, puppetModelPaths paths) : Geo_Puppet(prefix){} // TODO: Generate actors with model data

protected:
    Geo_NodeActor* actors[PUPPET_PARTS];
    Geo_Node* nodes[PUPPET_PARTS];
};

#endif

#define GEO_PUPPET_H
#endif