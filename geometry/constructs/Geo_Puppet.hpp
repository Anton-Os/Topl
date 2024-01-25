#ifndef GEO_PUPPET_H

#include "meshes/Geo_Shape2D.hpp"

#include "Geo_Construct.hpp"

#define PUPPET_PARTS 6

enum PUPPET_Part {
    PUPPET_Head = 0,
	PUPPET_LeftArm = 1,
	PUPPET_RightArm = 2,
	PUPPET_Body = 3,
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

    void configure(Topl_Scene* scene) override {
        scene->addGeometry(getPrefix() + "_" + "head", &_geoActors[PUPPET_Head]);
        scene->addGeometry(getPrefix() + "_" + "leftArm", &_geoActors[PUPPET_LeftArm]);
        scene->addGeometry(getPrefix() + "_" + "rightArm", &_geoActors[PUPPET_RightArm]);
        scene->addGeometry(getPrefix() + "_" + "torso", &_geoActors[PUPPET_Body]);
        scene->addGeometry(getPrefix() + "_" + "leftLeg", &_geoActors[PUPPET_LeftLeg]);
        scene->addGeometry(getPrefix() + "_" + "rightLeg", &_geoActors[PUPPET_RightLeg]);
    }
};

#ifdef RASTERON_H

typedef const std::string puppetSpritePaths[PUPPET_PARTS]; // list of paths for loading sprites

class Geo_Puppet2D : public Geo_Puppet {
public:
    // Geo_Puppet2D(const std::string& prefix) : Geo_Puppet(prefix){}
    Geo_Puppet2D(const std::string& prefix, puppetSpritePaths spriteImgPaths) : Geo_Puppet(prefix){
        _geoActors.resize(PUPPET_PARTS);
#ifdef RASTERON_H
        for(unsigned p = 0; p < PUPPET_PARTS; p++){
            spriteImgs[p].setFileImage(spriteImgPaths[p].c_str());
            sprites[p] = loadSprite(spriteImgs[p].getImage());
            quads[p].scale({ // scale to sprite size
                (float)sprites[p]->bounds.botRight[0] - (float)sprites[p]->bounds.botLeft[0], // x
                (float)sprites[p]->bounds.topRight[1] - (float)sprites[p]->bounds.botRight[1], // y
                0.0f
            });
            _geoActors[p].setMesh(&quads[p]);
        }
#endif
    }
    // Geo_Puppet2D(const std::string& prefix, const std::string& fileImgs[PUPPET_PARTS], Topl_Scene* scene) : Geo_Puppet(prefix, &actors, scene){}

#ifdef RASTERON_H
    ~Geo_Puppet2D(){ }
#endif

    void configure(Topl_Scene* scene) override {
        Geo_Puppet::configure(scene);
        for(unsigned p = 0; p < PUPPET_PARTS; p++)
            scene->addTexture(_geoActors[p].getName(), &spriteImgs[p]);
    }

protected:
    Geo_Quad2D quads[PUPPET_PARTS];
#ifdef RASTERON_H  
    Img_Base spriteImgs[PUPPET_PARTS];
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
        // auto headActor = std::find_if(_geoNodes.begin(); _geoNodes.end(); [](const Geo_NodeActor*& nodeActor){ return nodeActor->getName.find("head") != std::string::npos; });
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
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "torso", actors[PUPPET_Body]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "leftLeg", actors[PUPPET_LeftLeg]);
        scene->addGeometry(Geo_Puppet::getPrefix() + "_" + "rightLeg", actors[PUPPET_RightLeg]);

        // TODO: Add texture support
    }

protected:
    Geo_NodeActor* actors[PUPPET_PARTS]; // TODO: Replace with _geoActors from within construct
};

#endif

#define GEO_PUPPET_H
#endif