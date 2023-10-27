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
    Geo_Puppet(const std::string& prefix, puppetBlocks parts) : Geo_Construct(prefix){}
    Geo_Puppet(const std::string& prefix, puppetBlocks parts, Topl_Scene* scene) : Geo_Construct(prefix){
        configure(scene);
    }

    void configure(Topl_Scene* scene) override{
        // TODO: Add body here
    }
};

#ifdef RASTERON_H

class Geo_Puppet2D : public Geo_Puppet(){
    // TODO: Add body here

    Rasteron_Sprite* sprites[PUPPET_PARTS];
}

#endif
#ifdef ASSIMP_FOUND

class Geo_Puppet3D : public Geo_Puppet(){
    // TODO: Add body here

    Geo_Node* nodes[PUPPET_PARTS];
}

#endif

#define GEO_PUPPET_H
#endif