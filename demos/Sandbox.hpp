#include "meshes/Geo_Shape2D.hpp"
#include "meshes/Geo_Conic.hpp"
#include "meshes/Geo_Volume3D.hpp"
#include "meshes/Geo_Sphere.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"
// #include "constructs/Geo_Model.hpp"
#include "constructs/Geo_Layout.hpp"
#include "constructs/Geo_Puppet.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"

#include "Topl_Main.hpp"

struct Sandbox_Demo : public Topl_Main {
    Sandbox_Demo(const char* execPath, TARGET_Backend backend) 
    : Topl_Main(execPath, "Sandbox", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
        beamPipeline = Topl_Factory::genPipeline(backend, &beamVShader, &beamPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Topl_Scene scene; // for main elements
    Topl_Scene gui; // for gui elements
    Topl_Scene details; // for details elements
private:
    Topl_Pipeline *flatPipeline, *texPipeline, *beamPipeline;

    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
    Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
    Beams_VertexShader_GL4 beamVShader; Beams_PixelShader_GL4 beamPShader;

    Geo_VolumeQuad boxMesh = Geo_VolumeQuad();
    Geo_Actor boxActor = Geo_Actor(&boxMesh);
    Geo_ConicTriangle pyramidMesh = Geo_ConicTriangle();
    Geo_Actor pyramidActor = Geo_Actor(&pyramidMesh);
    Geo_Sphere sphereMesh = Geo_Sphere();
    Geo_Actor sphereActor = Geo_Actor(&sphereMesh);
    Geo_ShapeHex hexMesh = Geo_ShapeHex();
    Geo_Actor hexActor = Geo_Actor(&hexMesh);
};