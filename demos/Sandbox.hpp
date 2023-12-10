#include "meshes/Geo_Shape2D.hpp"
#include "meshes/Geo_Shape3D.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Sphere.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"
#include "constructs/Geo_Layout.hpp"
#include "constructs/Geo_Puppet.hpp"
// #include "constructs/Geo_Model.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Effect_Shader.hpp"

#include "Topl_Main.hpp"

struct Sandbox_Demo : public Topl_Main {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) 
    : Topl_Main(execPath, "Sandbox", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
        beamPipeline = Topl_Factory::genPipeline(backend, &beamVShader, &beamPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Topl_Scene scene; // for main elements
    Topl_Scene overlay; // for gui elements
    Topl_Scene details; // for details elements
private:
    Topl_Pipeline *flatPipeline, *texPipeline, *beamPipeline;

    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
    Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
    Beams_VertexShader_GL4 beamVShader; Beams_PixelShader_GL4 beamPShader;

    Geo_Quad3D boxMesh = Geo_Quad3D();
    Geo_Actor boxActor = Geo_Actor(&boxMesh);
    Geo_Triangle2D pyramidMesh = Geo_Triangle2D();
    Geo_Actor pyramidActor = Geo_Actor(&pyramidMesh);
    Geo_Sphere sphereMesh = Geo_Sphere();
    Geo_Actor sphereActor = Geo_Actor(&sphereMesh);
    Geo_HexCone hexMesh = Geo_HexCone();
    Geo_Actor hexActor = Geo_Actor(&hexMesh);

    Geo_Chain chain = Geo_Chain("chain", &sphereMesh, Vec3f({0.0F, 0.1F, 0.0F}), 9);
    Geo_Grid grid = Geo_Grid("grid", &boxMesh, Geo_Grid_Params(std::make_pair(3, 0.25F)));
    
    Geo_GridLayout layout1 = Geo_GridLayout("gridLayout", 3);
    Geo_VertLayout layout2 = Geo_VertLayout("vertLayout", 9);
    Geo_HorzLayout layout3 = Geo_HorzLayout("horzLayout", 9);
};