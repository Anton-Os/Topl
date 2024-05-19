#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define ENTROPIC_SPAWN 100
#define ENTROPIC_COUNT 1500

struct Entropy_Demo : public Topl_Program {
    Entropy_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Entropy", backend){}

    void init() override;
    void loop(double frameTime) override;

    Geo_Triangle2D triangles[ENTROPIC_COUNT / 4];
    Geo_Quad2D quads[ENTROPIC_COUNT / 4];
    Geo_Hex2D hexes[ENTROPIC_COUNT / 4];
    Geo_Circle2D circles[ENTROPIC_COUNT / 4];
    Geo_Orb spheres[ENTROPIC_SPAWN]; // for spawning objects

    Geo_Actor actors[ENTROPIC_COUNT];
    Geo_Actor spawnActors[ENTROPIC_SPAWN];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    Topl_Scene scene;
    Topl_Scene spawnScene;
} *_DEMO;