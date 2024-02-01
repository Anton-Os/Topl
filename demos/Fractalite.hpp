#include "meshes/Geo_Shape2D.hpp"
#include "meshes/Geo_Shape3D.hpp"

#include "Flat_Shader.hpp"

#include "Topl_Program.hpp"

#define FRACTALITE_COUNT 1000

struct Fractalite_Demo : public Topl_Program {
    Fractalite_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Fractalite", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Triangle3D triangles[FRACTALITE_COUNT];
    Geo_Quad3D quads[FRACTALITE_COUNT];
    Geo_Hex3D hexes[FRACTALITE_COUNT];
    Geo_Circle3D circles[FRACTALITE_COUNT];

    Geo_Actor actors[FRACTALITE_COUNT];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[FRACTALITE_COUNT];
#endif
private:
    Topl_Scene scene;

    Topl_Pipeline* flatPipeline; // flatPipeline;
    Flat_VertexShader_DX11 flatVShader; Flat_PixelShader_DX11 flatPShader;
} *_instance;