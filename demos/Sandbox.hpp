#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Orb.hpp"
#include "meshes/Geo_Parametric.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"
#include "constructs/Geo_Billboards.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Effect_Shader.hpp"

#include "program/Topl_Program.hpp"

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        beamPipeline = Topl_Factory::genPipeline(backend, &beamVShader, &beamPShader);
        effectPipeline = Topl_Factory::genPipeline(backend, &effectVShader, &effectPShader);
        texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
    }
    ~Sandbox_Demo();

    void init() override;
    void loop(double frameTime) override;

    static int mode;
    static int pipelineIndex;
    static bool isShaderVariant;
    static unsigned shaderMode;
    static Vec3f texScroll;
    static std::string fontFilePath;

    Geo_Quad2D canvasMesh = Geo_Quad2D(10000.0);
    Geo_Actor canvasActor = Geo_Actor(&canvasMesh);

    Geo_Quad3D boxMesh = Geo_Quad3D();
    Geo_Actor boxActor = Geo_Actor(&boxMesh);
    Geo_Triangle3D pyramidMesh = Geo_Triangle3D();
    Geo_Actor pyramidActor = Geo_Actor(&pyramidMesh);
    Geo_Orb sphereMesh = Geo_Orb();
    Geo_Actor sphereActor = Geo_Actor(&sphereMesh);
    Geo_HexCone hexMesh = Geo_HexCone();
    Geo_Actor hexActor = Geo_Actor(&hexMesh);
    Geo_ParametricSet paramMeshes[3] = {
        Geo_ParametricSet({{ 0.0f, 0.0f, 0.0f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }, 
                           { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.0f, 0.0f }, 
                           { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.35f, 0.35f }, 
                           { 0.0f, -0.35f, 0.35f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }
        }), // basic constructor
        Geo_ParametricSet({{ 0.0f, 0.0f, 0.0f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }, 
                           { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.0f, 0.0f }, 
                           { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.35f, 0.35f }, 
                           { 0.0f, -0.35f, 0.35f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }
                        }, { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.25F, 0.0f }, { 0.0f, -0.25F, 0.0f }, }), // paths constuctor
        Geo_ParametricSet({{ 0.0f, 0.0f, 0.0f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }, 
                           { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.0f, 0.0f }, 
                           { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.35f, 0.35f }, 
                           { 0.0f, -0.35f, 0.35f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }
                        },  0.8F, 5) // insertion constructor
    };
    Geo_Actor paramActors[3] = { Geo_Actor(&paramMeshes[0]), Geo_Actor(&paramMeshes[1]), Geo_Actor(&paramMeshes[2]) };

    Geo_Chain chain = Geo_Chain("chain", &sphereMesh, Vec3f({0.0F, 0.1F, 0.0F}), 9);
    Geo_Grid grid = Geo_Grid("grid", &boxMesh, Geo_Grid_Params(std::make_pair(3, 0.25F)));
    
    Geo_Quad2D timerInfoMesh = Geo_Quad2D(0.15);
    Geo_Actor timerInfoActor = Geo_Actor(&timerInfoMesh);
    Geo_Quad2D pickerInfoMesh = Geo_Quad2D(0.15);
    Geo_Actor pickerInfoActor = Geo_Actor(&pickerInfoMesh);
    Geo_Listboard modeLayout = Geo_Listboard("modeLayout", 6);
    Geo_Paneboard timelineLayout = Geo_Paneboard("timelineLayout");
    Geo_Gridboard actionsLayout = Geo_Gridboard("actionsLayout", 3);
    Geo_Crossboard statusLayout = Geo_Crossboard("statusLayout", 6);

#ifdef RASTERON_H
    Img_Base canvasTex;

    Img_Base timerCount_texture = Img_Base(0xFFFFFF00);
    Img_Base pickerObj_texture = Img_Base(0xFFFFFF00);
    Rasteron_Image *boxImg, *pyramidImg, *sphereImg, *hexImg, *paramImg;
    Img_Base boxTex, pyramidTex, sphereTex, hexTex, paramTex;

    Img_Button checks[3] = { Img_Button(MENU_XL), Img_Button(MENU_XL), Img_Button(MENU_XL) };
    Img_Dial dials[3] = { Img_Dial(MENU_XL, 4), Img_Dial(MENU_XL, 8), Img_Dial(MENU_XL, 16) }; 
    Img_Button icons[3] = { Img_Button(MENU_XL, "arrow_back"), Img_Button(MENU_XL, "sync"), Img_Button(MENU_XL, "arrow_forward") };

    Img_Slider timelineSlider = Img_Slider(MENU_XL, 60); Img_Base timelineTex = Img_Base(timelineSlider.stateImg.getImage());
    Img_Base modeButtons[6] = { Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111) };
    Img_Button actionButtons[9] = { 
        Img_Button(MENU_Small, "zoom_in"), Img_Button(MENU_Small, "arrow_downward"), Img_Button(MENU_Small, "zoom_out"),
        Img_Button(MENU_Small, "arrow_back"), Img_Button(MENU_Small, "cancel"), Img_Button(MENU_Small, "arrow_forward"),
        Img_Button(MENU_Small, "rotate_left"), Img_Button(MENU_Small, "arrow_upward"), Img_Button(MENU_Small, "rotate_right"), 
    };
    Img_Base statusButtons[6] = { Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111) };

    Img_Slider sliders[9] = { Img_Slider(MENU_XL, 2), Img_Slider(MENU_XL, 3), Img_Slider(MENU_XL, 4), Img_Slider(MENU_XL, 5), Img_Slider(MENU_XL, 6), Img_Slider(MENU_XL, 7), Img_Slider(MENU_XL, 8), Img_Slider(MENU_XL, 9), Img_Slider(MENU_XL, 10) };
    Rasteron_Queue* words_queue = alloc_queue("words", { 64, 512 }, 9);
    Img_Base words_textures[9] = { getFrameAt(words_queue, 0), getFrameAt(words_queue, 1), getFrameAt(words_queue, 2), getFrameAt(words_queue, 3), getFrameAt(words_queue, 4), getFrameAt(words_queue, 5), getFrameAt(words_queue, 6), getFrameAt(words_queue, 7), getFrameAt(words_queue, 8) };
#endif
private:
    Topl_Scene canvas; // for backdrop element
    Topl_Scene scene; // for main elements
    Topl_Scene overlay; // for gui elements
    Topl_Scene details; // for details elements

    Topl_Camera fixedCamera;

    Topl_Pipeline *flatPipeline, *texPipeline, *beamPipeline, *effectPipeline;

    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
    Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
    Beams_VertexShader_GL4 beamVShader; Beams_PixelShader_GL4 beamPShader;
    Effect_VertexShader_GL4 effectVShader; Effect_PixelShader_GL4 effectPShader;
} *_instance; // USE _instance OBJECT FOR DEMO