#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Iterable.hpp"

#include "program/Topl_Program.hpp"

#define ENTROPIC_SPAWN 100
#define ENTROPIC_COUNT 200
#define ENTROPIC_PROB 0.05
#define ENTROPIC_SIZE 0.045
#define ENTROPIC_FORCE 50000.0F

struct Entropy_Demo : public Topl_Program {
    Entropy_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Entropy", backend){
        backdropActor.setPos({ 0.0F, 0.0F, -0.1F });
        for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
            switch(a % 4){
                case 0: actors[a] = Geo_Actor(&trigs3D[a / 4]); break;
                case 1: actors[a] = Geo_Actor(&quads3D[a / 4]); break;
                case 2: actors[a] = Geo_Actor(&hexes3D[a / 4]); break;
                case 3: actors[a] = Geo_Actor(&circles3D[a / 4]); break;
            }
            actors[a].setName("actor" + std::to_string(a));
            actors[a].setPos({ (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, 0.0 });
            actors[a].setRot({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, });
            float size = (float)rand() / (float)RAND_MAX * ENTROPIC_SIZE;
            actors[a].setSize({ (size > 0.01F)? size : 0.01F, (size > 0.01F)? size : 0.01F, (size > 0.01F)? size : 0.01F });
            
            /* 
            for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
            switch(a % 4){
                case 0: volumeMeshes.push_back(new Geo_Ext3D({ trigs3D[a / 4].getRadius(), trigs3D[a / 4].getSegments() }, trigs3D[a / 4].getDepth(), 1)); break;
                case 1: volumeMeshes.push_back(new Geo_Ext3D({ quads3D[a / 4].getRadius(), quads3D[a / 4].getSegments() }, quads3D[a / 4].getDepth(), 1)); break;
                case 2: volumeMeshes.push_back(new Geo_Ext3D({ hexes3D[a / 4].getRadius(), hexes3D[a / 4].getSegments() }, hexes3D[a / 4].getDepth(), 1)); break;
                case 3: volumeMeshes.push_back(new Geo_Ext3D({ circles3D[a / 4].getRadius(), circles3D[a / 4].getSegments() }, circles3D[a / 4].getDepth(), 1)); break;
            }
            
            extActors[a] = Geo_Actor(volumeMeshes.back());
            extActors[a].setPos(*actors[a].getPos());
            extActors[a].setRot(*actors[a].getRot());
            extActors[a].setSize(*actors[a].getSize()); */
        }
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;

    Topl_Scene* getScene(){
        switch(mode % 3){
            case 1: return &scene2;
            case 2: return &scene3;
            default: return &scene1;
        }
    }

    Geo_Quad2D backdropMesh = Geo_Quad2D(100.0);
    Geo_Actor backdropActor = Geo_Actor(&backdropMesh);

    Geo_Trig2D trigs[ENTROPIC_COUNT / 4]; Geo_TrigCone trigsCones[ENTROPIC_COUNT / 4]; Geo_Trig3D trigs3D[ENTROPIC_COUNT / 4];
    Geo_Quad2D quads[ENTROPIC_COUNT / 4]; Geo_QuadCone quadsCones[ENTROPIC_COUNT / 4]; Geo_Quad3D quads3D[ENTROPIC_COUNT / 4];
    Geo_Hex2D hexes[ENTROPIC_COUNT / 4]; Geo_Hex3D hexesCones[ENTROPIC_COUNT / 4]; Geo_Hex3D hexes3D[ENTROPIC_COUNT / 4];
    Geo_Circle2D circles[ENTROPIC_COUNT / 4]; Geo_CircleCone circlesCones[ENTROPIC_COUNT / 4]; Geo_Circle3D circles3D[ENTROPIC_COUNT / 4];

    std::vector<Geo_Ext2D*> surfaceMeshes; std::vector<Geo_ExtCone*> coneMeshes; std::vector<Geo_Ext3D*> volumeMeshes;

    Geo_Actor actors[ENTROPIC_COUNT];
    Geo_Actor extActors[ENTROPIC_COUNT];
    /* Geo_Actor tessActors[ENTROPIC_COUNT];
    Geo_Actor duplexActors[ENTROPIC_COUNT]; */
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    Topl_Scene scene1, scene2, scene3;
    Topl_Scene backdropScene;
    // Topl_Scene spawnScene;
} *_DEMO;