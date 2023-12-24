#include "meshes/Geo_Shape2D.hpp"
#include "meshes/Geo_Shape3D.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Sphere.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"
#include "constructs/Geo_Layout.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Effect_Shader.hpp"

#include "Topl_Program.hpp"

#define SANDBOX_MODE_HIDE 0 // object funtion
#define SANDBOX_MODE_MOVE 1 // object function
#define SANDBOX_MODE_ROTATE 2 // object function
#define SANDBOX_MODE_SCALE 3 // object function
#define SANDBOX_MODE_REWIND 4 // timer function
#define SANDBOX_MODE_PLAYPAUSE 5 // timer function
#define SANDBOX_MODE_FORWARD 6 // timer funtion
#define SANDBOX_MODE_PAN 7 // scene function
#define SANDBOX_MODE_LOOK 8 // scene function
#define SANDBOX_MODE_ZOOM 9 // scene function

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        beamPipeline = Topl_Factory::genPipeline(backend, &beamVShader, &beamPShader);
        effectPipeline = Topl_Factory::genPipeline(backend, &effectVShader, &effectPShader);
        texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
    }

    void init() override;
    void loop(double frameTime) override;

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
private:
    Topl_Scene scene; // for main elements
    Topl_Scene overlay; // for gui elements
    Topl_Scene details; // for details elements

    Topl_Pipeline *flatPipeline, *texPipeline, *beamPipeline, *effectPipeline;

    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
    Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
    Beams_VertexShader_GL4 beamVShader; Beams_PixelShader_GL4 beamPShader;
    Effect_VertexShader_GL4 effectVShader; Effect_PixelShader_GL4 effectPShader;

} *_instance; // USE _instance OBJECT FOR DEMO