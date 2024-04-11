#include "meshes/Geo_Volume.hpp"

#include "Flat_Shader.hpp"

#include "program/Topl_Program.hpp"

struct Builder_Demo : public Topl_Program {
    Builder_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Builder", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Quad3D bigBox = Geo_Quad3D(30.0f);
    Geo_Actor bigBoxActor = Geo_Actor(&bigBox);
private:
    Topl_Scene scene;

    Topl_Pipeline* flatPipeline;
    Flat_VertexShader_DX11 flatVShader; Flat_PixelShader_DX11 flatPShader;
} *_instance;