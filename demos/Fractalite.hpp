#include "meshes/Geo_Shape2D.hpp"
#include "meshes/Geo_Shape3D.hpp"

#include "Flat_Shader.hpp"

#include "Topl_Program.hpp"

#define FRACTALITE_COUNT 1024

struct Fractalite_Demo : public Topl_Program {
    Fractalite_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Fractalite", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Triangle2D triangles[FRACTALITE_COUNT];
    Geo_Quad2D quads[FRACTALITE_COUNT];
    Geo_Hex2D hexes[FRACTALITE_COUNT];
    Geo_Circle2D circles[FRACTALITE_COUNT];

    Geo_Actor actors[FRACTALITE_COUNT];
private:
    Topl_Scene scene;

    Topl_Pipeline* flatPipeline; // flatPipeline;
    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
} *_instance;