#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orb.hpp"

#include "Flat_Shader.hpp"

#include "Topl_Program.hpp"

#define ENTROPIC_SPAWN 100
#define ENTROPIC_COUNT 1100

struct Entropy_Demo : public Topl_Program {
    Entropy_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Entropy", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Triangle3D triangles[ENTROPIC_COUNT / 4];
    Geo_Quad3D quads[ENTROPIC_COUNT / 4];
    Geo_Hex3D hexes[ENTROPIC_COUNT / 4];
    Geo_Circle3D circles[ENTROPIC_COUNT / 4];
    Geo_Orb spheres[ENTROPIC_SPAWN]; // for spawning objects

    Geo_Actor actors[ENTROPIC_COUNT];
    Geo_Actor spawnActors[ENTROPIC_SPAWN];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    Topl_Scene scene;
    Topl_Scene spawnScene;

    Topl_Pipeline* flatPipeline;
    Flat_VertexShader_DX11 flatVShader; Flat_PixelShader_DX11 flatPShader;
} *_instance;