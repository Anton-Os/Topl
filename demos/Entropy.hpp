#include "meshes/Geo_Surface.hpp"
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

            // TODO: Create meshes
            // TODO: Create tesselated meshes
            // TODO: Create duplex meshes
        }
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Quad2D backdropMesh = Geo_Quad2D(100.0);
    Geo_Actor backdropActor = Geo_Actor(&backdropMesh);

    Geo_Trig3D triangles[ENTROPIC_COUNT / 4];
    Geo_Quad3D quads[ENTROPIC_COUNT / 4];
    Geo_Hex3D hexes[ENTROPIC_COUNT / 4];
    Geo_Circle3D circles[ENTROPIC_COUNT / 4];
    // Geo_Orb spheres[ENTROPIC_SPAWN]; // for spawning objects

    std::vector<Geo_Mesh> meshes;
    std::vector<Geo_TessIter> tessMeshes;
    std::vector<Geo_DuplexIter> duplexMeshes;

    Geo_Actor actors[ENTROPIC_COUNT];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    Topl_Scene scene;
    Topl_Scene backdropScene;
    // Topl_Scene spawnScene;
} *_DEMO;