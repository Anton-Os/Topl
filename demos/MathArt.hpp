#include "meshes/Geo_Volume.hpp"

#include "Effect_Shader.hpp"
#include "Flat_Shader.hpp"

#include "program/Topl_Program.hpp"

struct MathArt_Demo : public Topl_Program {
    MathArt_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "MathArt", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        effectPipeline = Topl_Factory::genPipeline(backend, &effectVShader, &effectPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Circle3D circle = Geo_Circle3D(1.25);
    Geo_Actor circleActor = Geo_Actor(&circle);
    Geo_Triangle3D triangle = Geo_Triangle3D(1.25);
    Geo_Actor triangleActor = Geo_Actor(&triangle);
    Geo_Quad3D box = Geo_Quad3D(1.25);
    Geo_Actor boxActor = Geo_Actor(&box);
private:
    Topl_Scene scene;

    Topl_Pipeline* flatPipeline;
    Topl_Pipeline* effectPipeline;

    Flat_VertexShader_DX11 flatVShader; Flat_PixelShader_DX11 flatPShader;
    Effect_VertexShader_DX11 effectVShader; Effect_PixelShader_DX11 effectPShader;
} *_instance;