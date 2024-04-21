#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Orb.hpp"
#include "meshes/Geo_Parametric.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"
#include "constructs/Geo_Billboards.hpp"

#include "program/Topl_Program.hpp"

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){}
    ~Sandbox_Demo();

    void init() override;
    void loop(double frameTime) override;

    static int mode;
    static int pipelineIndex;
    static bool isShaderVariant;
    static bool isDetailsShown;
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
    Geo_ParametricSet paramMesh = Geo_ParametricSet({
        { 0.0f, 0.0f, 0.0f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }, 
        { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.0f, 0.0f }, 
        { -0.25F, -0.25F, 0.0f }, { 0.25F, -0.25F, 0.0f }, { 0.0f, 0.35f, 0.35f }, 
        { 0.0f, -0.35f, 0.35f }, { 0.25F, 0.25F, 0.0f }, { -0.25F, 0.25F, 0.0f }
    }); // basic constructor
    Geo_Actor paramActor = Geo_Actor(&paramMesh);

    Geo_Quad2D timerInfoMesh = Geo_Quad2D(0.15);
    Geo_Actor timerInfoActor = Geo_Actor(&timerInfoMesh);
    Geo_Quad2D pickerInfoMesh = Geo_Quad2D(0.15);
    Geo_Actor pickerInfoActor = Geo_Actor(&pickerInfoMesh);
    Geo_Quad2D sceneInfoMesh = Geo_Quad2D(0.15);
    Geo_Actor sceneInfoActor = Geo_Actor(&pickerInfoMesh);
    Geo_Listboard modeLayout = Geo_Listboard("modeLayout", 6);
    Geo_Paneboard timelineLayout = Geo_Paneboard("timelineLayout");
    Geo_Gridboard actionsLayout = Geo_Gridboard("actionsLayout", 3);
    Geo_Crossboard pickerPropsLayout = Geo_Crossboard("pickerPropsLayout", 6);
    Geo_Crossboard scenePropsLayout = Geo_Crossboard("scenePropsLayout", 6);

    Geo_Quad2D labels[100]; Img_Base labelsTex[100];
    Geo_Triangle2D arrows[100]; Img_Base arrowsTex[100];
    Geo_Circle2D dots[100]; Img_Base dotsTex[100];
    Geo_Actor detailActors[100 + 100 + 100];
#ifdef RASTERON_H
    Img_Base canvasTex;

    Img_Base timerInfo_texture = Img_Base(0xFFFFFF00);
    Img_Base pickerInfo_texture = Img_Base(0xFFFFFF00);
    Img_Base sceneInfo_texture = Img_Base(0xFFFFFF00);
    Rasteron_Image *boxImg, *pyramidImg, *sphereImg, *hexImg, *paramImg;
    Img_Base boxTex, pyramidTex, sphereTex, hexTex, paramTex;

    Img_Button checks[3] = { Img_Button(MENU_XL), Img_Button(MENU_XL), Img_Button(MENU_XL) };
    Img_Dial dials[3] = { Img_Dial(MENU_XL, 4), Img_Dial(MENU_XL, 8), Img_Dial(MENU_XL, 16) }; 
    Img_Button icons[3] = { Img_Button(MENU_XL, "arrow_back"), Img_Button(MENU_XL, "sync"), Img_Button(MENU_XL, "arrow_forward") };

    Img_Slider timelineSlider = Img_Slider(MENU_XL, 60); Img_Base timelineTex = Img_Base(timelineSlider.stateImg.getImage());
    Img_Base modeButtons[6] = { Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111) };
    Img_Button actionButtons[9] = { 
        Img_Button(MENU_Large, "zoom_in"), Img_Button(MENU_Large, "arrow_downward"), Img_Button(MENU_Large, "zoom_out"),
        Img_Button(MENU_Large, "arrow_back"), Img_Button(MENU_Large, "cancel"), Img_Button(MENU_Large, "arrow_forward"),
        Img_Button(MENU_Large, "rotate_left"), Img_Button(MENU_Large, "arrow_upward"), Img_Button(MENU_Large, "rotate_right"), 
    };
    Img_Base propsRoot = Img_Base(0xFF666666);
    Img_Base propsPane = Img_Base(0xAAEEEEEE);
    Img_Button scenePropBtns[6] = {
        Img_Button(MENU_Medium, "add"), Img_Button(MENU_Medium, "build"), Img_Button(MENU_Medium, "code"),
        Img_Button(MENU_Medium, "cached"), Img_Button(MENU_Medium, "crop_free"), Img_Button(MENU_Medium, "delete"),
    };
    Img_Button pickerPropBtns[6] = {
        Img_Button(MENU_Medium), Img_Button(MENU_Medium), Img_Button(MENU_Medium),
        Img_Button(MENU_Medium), Img_Button(MENU_Medium), Img_Button(MENU_Medium),
    };
    // Img_Base statusButtons[6] = { Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111), Img_Base(0xFF111111) };

    Img_Slider sliders[9] = { Img_Slider(MENU_XL, 2), Img_Slider(MENU_XL, 3), Img_Slider(MENU_XL, 4), Img_Slider(MENU_XL, 5), Img_Slider(MENU_XL, 6), Img_Slider(MENU_XL, 7), Img_Slider(MENU_XL, 8), Img_Slider(MENU_XL, 9), Img_Slider(MENU_XL, 10) };
    Rasteron_Queue* words_queue = RASTERON_QUEUE_ALLOC("words", createImgSize(64, 512), 9);
    Img_Base words_textures[9] = { queue_getImg(words_queue, 0), queue_getImg(words_queue, 1), queue_getImg(words_queue, 2), queue_getImg(words_queue, 3), queue_getImg(words_queue, 4), queue_getImg(words_queue, 5), queue_getImg(words_queue, 6), queue_getImg(words_queue, 7), queue_getImg(words_queue, 8) };
#endif
private:
    Topl_Scene canvas; // for backdrop element
    Topl_Scene scene; // for main elements
    Topl_Scene overlay; // for gui elements
    Topl_Scene details; // for details elements

    Topl_Camera fixedCamera;
} *_instance; // USE _instance OBJECT FOR DEMO