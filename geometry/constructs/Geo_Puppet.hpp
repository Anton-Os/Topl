#ifndef GEO_PUPPET_H

#include "meshes/Geo_Surface.hpp"

#include "Geo_Construct.hpp"

#define PUPPET_PARTS 6
#define PUPPET_LINKS 10

enum PUPPET_Part {
    PUPPET_Body = 0,
    PUPPET_Head = 1,
	PUPPET_LeftArm = 2,
	PUPPET_RightArm = 3,
	PUPPET_LeftLeg = 4,
	PUPPET_RightLeg = 5
};

typedef const Geo_Actor* puppetBlocks[PUPPET_PARTS];

struct Geo_Puppet : public Geo_Construct<Geo_Puppet> {
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

    void init() override { } // should this be overriden if empty?

    void configure(Topl_Scene* scene) override {
        scene->addGeometry(getPrefix() + "head", &_geoActors[PUPPET_Head]);
        scene->addGeometry(getPrefix() + "body", &_geoActors[PUPPET_Body]);
        scene->addGeometry(getPrefix() + "leftArm", &_geoActors[PUPPET_LeftArm]);
        scene->addGeometry(getPrefix() + "rightArm", &_geoActors[PUPPET_RightArm]);
        scene->addGeometry(getPrefix() + "leftLeg", &_geoActors[PUPPET_LeftLeg]);
        scene->addGeometry(getPrefix() + "rightLeg", &_geoActors[PUPPET_RightLeg]);
    }

#ifdef TOPL_ENABLE_PHYSICS
    void addPhysics(Topl_Scene* scene){
        // Adding Physics
        _physActors.resize(PUPPET_PARTS);
        for(unsigned p = 0; p < PUPPET_PARTS; p++) scene->addPhysics(_geoActors[p].getName(), &_physActors[p]);
        _links.resize(PUPPET_PARTS + 5);
        // for(unsigned l = 0; l < PUPPET_PARTS + 5; l++) _links[l].kVal = PHYS_ROD_K * 5;
        // Star Links
        _links[PUPPET_Head].preset(*_geoActors[PUPPET_Head].getPos(), *_geoActors[PUPPET_Body].getPos());
        scene->addLink(&_links[PUPPET_Head], getPrefix() + "head", getPrefix() + "body");
        _links[PUPPET_LeftArm].preset(*_geoActors[PUPPET_LeftArm].getPos(), *_geoActors[PUPPET_Body].getPos());
        scene->addLink(&_links[PUPPET_LeftArm], getPrefix() + "leftArm", getPrefix() + "body");
        _links[PUPPET_RightArm].preset(*_geoActors[PUPPET_RightArm].getPos(), *_geoActors[PUPPET_Body].getPos());
        scene->addLink(&_links[PUPPET_RightArm], getPrefix() + "rightArm", getPrefix() + "body");
        _links[PUPPET_LeftLeg].preset(*_geoActors[PUPPET_LeftLeg].getPos(), *_geoActors[PUPPET_Body].getPos());
        scene->addLink(&_links[PUPPET_LeftLeg], getPrefix() + "leftLeg", getPrefix() + "body");
        _links[PUPPET_RightLeg].preset(*_geoActors[PUPPET_RightLeg].getPos(), *_geoActors[PUPPET_Body].getPos());
        scene->addLink(&_links[PUPPET_RightLeg], getPrefix() + "rightLeg", getPrefix() + "body");
        // Pentagonal Links
        _links[PUPPET_PARTS].preset(*_geoActors[PUPPET_RightLeg].getPos(), *_geoActors[PUPPET_LeftLeg].getPos());
        scene->addLink(&_links[PUPPET_PARTS], getPrefix() + "rightLeg", getPrefix() + "leftLeg");
        _links[PUPPET_PARTS + 1].preset(*_geoActors[PUPPET_RightLeg].getPos(), *_geoActors[PUPPET_RightArm].getPos());
        scene->addLink(&_links[PUPPET_PARTS + 1], getPrefix() + "rightLeg", getPrefix() + "rightArm");
        _links[PUPPET_PARTS + 2].preset(*_geoActors[PUPPET_LeftLeg].getPos(), *_geoActors[PUPPET_LeftArm].getPos());
        scene->addLink(&_links[PUPPET_PARTS + 2], getPrefix() + "leftLeg", getPrefix() + "leftArm");
        _links[PUPPET_PARTS + 3].preset(*_geoActors[PUPPET_Head].getPos(), *_geoActors[PUPPET_RightArm].getPos());
        scene->addLink(&_links[PUPPET_PARTS + 3], getPrefix() + "head", getPrefix() + "rightArm");
        _links[PUPPET_PARTS + 4].preset(*_geoActors[PUPPET_Head].getPos(), *_geoActors[PUPPET_LeftArm].getPos());
        scene->addLink(&_links[PUPPET_PARTS + 4], getPrefix() + "head", getPrefix() + "leftArm");
    }
#endif
};

#ifdef TOPL_ENABLE_TEXTURES

typedef std::string puppetSpritePaths[PUPPET_PARTS]; // list of paths for loading sprites

class Geo_Puppet2D : public Geo_Puppet {
public:
    // Geo_Puppet2D(const std::string& prefix) : Geo_Puppet(prefix){}
    Geo_Puppet2D(const std::string& prefix, puppetSpritePaths paths) : Geo_Puppet(prefix){
        for(unsigned p = 0; p < PUPPET_PARTS; p++) spriteImgPaths[p] = paths[p];
        init();
    }
    // Geo_Puppet2D(const std::string& prefix, const std::string& fileImgs[PUPPET_PARTS], Topl_Scene* scene) : Geo_Puppet(prefix, &actors, scene){}

#ifdef TOPL_ENABLE_TEXTURES
    // ~Geo_Puppet2D(){ for(unsigned p = 0; p < PUPPET_PARTS; p++) RASTERON_SPRITE_DEALLOC(sprites[p]); }
#endif

    void init() override {
        _geoActors.resize(PUPPET_PARTS);
#ifdef TOPL_ENABLE_TEXTURES
        for(unsigned p = 0; p < PUPPET_PARTS; p++){
            spriteImgs[p] = Sampler_File(spriteImgPaths[p].c_str());
            sprites[p] = loadSprite(spriteImgs[p].getImage());
            float width = bounds_getWidth(sprites[p]->bounds); float height = bounds_getHeight(sprites[p]->bounds);
            quads[p].scale({ width, height, 0.0f });
            switch(p){
                case PUPPET_Head: _geoActors[p].setPos({ 0.0F, (height / 2.0F) + (bounds_getHeight(sprites[PUPPET_Body]->bounds) / 4.0F), 0.0F }); break;
                case PUPPET_LeftArm: _geoActors[p].setPos({ width / -2.0F, height / 2.0F, 0.0F }); break;
                case PUPPET_RightArm: _geoActors[p].setPos({ width / 2.0F, height / 2.0F, 0.0F }); break;
                case PUPPET_LeftLeg: _geoActors[p].setPos({ width / -2.0F, (height / -2.0F) - (bounds_getHeight(sprites[PUPPET_Body]->bounds) / 4.0F), 0.0F }); break;
                case PUPPET_RightLeg: _geoActors[p].setPos({ width / 2.0F,  (height / -2.0F) - (bounds_getHeight(sprites[PUPPET_Body]->bounds) / 4.0F), 0.0F }); break;
                default: _geoActors[p].setPos({ 0.0F, 0.0F, 0.0F }); break;
            }
            _geoActors[p].setMesh(&quads[p]);
        }
#endif
    }

    void configure(Topl_Scene* scene) override {
        Geo_Puppet::configure(scene);
        for(unsigned p = 0; p < PUPPET_PARTS; p++)
            scene->addTexture(_geoActors[p].getName(), &spriteImgs[p]);
#ifdef TOPL_ENABLE_PHYSICS
        Geo_Puppet::addPhysics(scene);
#endif
    }

protected:
    puppetSpritePaths spriteImgPaths;
    Geo_Quad2D quads[PUPPET_PARTS];
#ifdef TOPL_ENABLE_TEXTURES  
    Sampler_2D spriteImgs[PUPPET_PARTS];
    Rasteron_Sprite* sprites[PUPPET_PARTS];
#endif
};

#endif
#ifdef TOPL_ENABLE_MODELS

#include "constructs/Geo_Model3D.hpp"

// typedef const std::string puppetModelPaths[PUPPET_PARTS]; // list of paths for loading models

class Geo_Puppet3D : public Geo_Puppet, public Geo_Model3D {
public:
    // Geo_Puppet3D(const std::string& prefix) : Geo_Puppet(prefix){}
    Geo_Puppet3D(const std::string& prefix, const std::string& modelPath) : Geo_Puppet(prefix), Geo_Model3D(prefix, modelPath) {
        init();
    }

    void init() override {
        // auto headActor = std::find_if(_geoNodes.begin(); _geoNodes.end(); [](const Geo_Node*& nodeActor){ return nodeActor->getName.find("head") != std::string::npos; });
        for(auto a = _geoNodes.begin(); a != _geoNodes.end(); a++){
            if((*a)->getName().find("Head") != std::string::npos || (*a)->getName().find("head") != std::string::npos)
                actors[PUPPET_Head] = *a; // finding head
            else if((*a)->getName().find("Body") != std::string::npos || (*a)->getName().find("body") != std::string::npos)
                actors[PUPPET_Body] = *a; // finding body
            else if ((*a)->getName().find("Arm") != std::string::npos || (*a)->getName().find("arm") != std::string::npos) {
                if ((*a)->getName().find("Left") != std::string::npos || (*a)->getName().find("left") != std::string::npos)
                    actors[PUPPET_LeftArm] = *a; // finding left arm
                else if ((*a)->getName().find("Right") != std::string::npos || (*a)->getName().find("right") != std::string::npos)
                    actors[PUPPET_RightArm] = *a; // finding right arm
            }
            else if ((*a)->getName().find("Leg") != std::string::npos || (*a)->getName().find("leg") != std::string::npos) {
                if ((*a)->getName().find("Left") != std::string::npos || (*a)->getName().find("left") != std::string::npos)
                    actors[PUPPET_LeftLeg] = *a; // finding left leg
                else if ((*a)->getName().find("Right") != std::string::npos || (*a)->getName().find("right") != std::string::npos)
                    actors[PUPPET_RightLeg] = *a; // finding right leg
            }
        }
    }

    void configure(Topl_Scene* scene) override {
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "head", actors[PUPPET_Head]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "leftArm", actors[PUPPET_LeftArm]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "rightArm", actors[PUPPET_RightArm]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "body", actors[PUPPET_Body]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "leftLeg", actors[PUPPET_LeftLeg]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "rightLeg", actors[PUPPET_RightLeg]);

        // TODO: Add texture support
    }

protected:
    Geo_Node* actors[PUPPET_PARTS]; // TODO: Replace with _geoActors from within construct
};

#endif

#define GEO_PUPPET_H
#endif
