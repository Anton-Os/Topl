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
                case 0: actors[a] = Geo_Actor(&triangles[a / 4]); break;
                case 1: actors[a] = Geo_Actor(&quads[a / 4]); break;
                case 2: actors[a] = Geo_Actor(&hexes[a / 4]); break;
                case 3: actors[a] = Geo_Actor(&circles[a / 4]); break;
            }
            actors[a].setName("actor" + std::to_string(a));
            actors[a].setPos({ (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, 0.0 });
            actors[a].setRot({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, });
            float size = (float)rand() / (float)RAND_MAX * ENTROPIC_SIZE;
            actors[a].setSize({ (size > 0.01F)? size : 0.01F, (size > 0.01F)? size : 0.01F, (size > 0.01F)? size : 0.01F });

            /* switch(a % 4){
                case 0: duplexMeshes.push_back(new Geo_DuplexIter(&triangles[a / 4])); tessMeshes.push_back(new Geo_TessIter(&triangles[a / 4], 1)); break;
                case 1: duplexMeshes.push_back(new Geo_DuplexIter(&quads[a / 4]));  tessMeshes.push_back(new Geo_TessIter(&quads[a / 4], 1)); break;
                case 2: duplexMeshes.push_back(new Geo_DuplexIter(&hexes[a / 4])); tessMeshes.push_back(new Geo_TessIter(&hexes[a / 4], 1)); break;
                case 3: duplexMeshes.push_back(new Geo_DuplexIter(&circles[a / 4])); tessMeshes.push_back(new Geo_TessIter(&circles[a / 4], 1)); break;
            } 

            tessActors[a] = Geo_Actor(tessMeshes.back());
            tessActors[a].setPos(*actors[a].getPos());
            tessActors[a].setRot(*actors[a].getRot());
            tessActors[a].setSize(*actors[a].getSize());
            duplexActors[a] = Geo_Actor(duplexMeshes.back());
            duplexActors[a].setPos(*actors[a].getPos());
            duplexActors[a].setRot(*actors[a].getRot());
            duplexActors[a].setSize(*actors[a].getSize()); */
        }
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;

    Topl_Scene* getScene(){
        switch(mode % 3){
            case 0: return &scene1;
            case 1: return &scene2;
            case 2: return &scene3;
        }
    }

    Geo_Quad2D backdropMesh = Geo_Quad2D(100.0);
    Geo_Actor backdropActor = Geo_Actor(&backdropMesh);

    Geo_Trig3D triangles[ENTROPIC_COUNT / 4];
    Geo_Quad3D quads[ENTROPIC_COUNT / 4];
    Geo_Hex3D hexes[ENTROPIC_COUNT / 4];
    Geo_Circle3D circles[ENTROPIC_COUNT / 4];
    // Geo_Orb orbs[ENTROPIC_COUNT / 4];

    // std::vector<Geo_Mesh> meshes;
    std::vector<Geo_TessIter*> tessMeshes;
    std::vector<Geo_DuplexIter*> duplexMeshes;

    Geo_Actor actors[ENTROPIC_COUNT];
    Geo_Actor tessActors[ENTROPIC_COUNT];
    Geo_Actor duplexActors[ENTROPIC_COUNT];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    Topl_Scene scene1, scene2, scene3;
    Topl_Scene backdropScene;
    // Topl_Scene spawnScene;
} *_DEMO;