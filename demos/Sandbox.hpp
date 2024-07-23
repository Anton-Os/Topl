#include "program/Topl_Program.hpp"

#include "meshes/Geo_Orb.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Billboards.hpp"

#define SANDBOX_ACTION_MENU 1
#define SANDBOX_OBJECT_MENU 2
#define SANDBOX_SCULPT_MENU 3
#define SANDBOX_PAINT_MENU 4

#define SANDBOX_PANE_COUNT 12
#define SANDBOX_EDITS_COUNT 10

struct Img_Canvas : public Img_Base {
    Img_Canvas(unsigned bk) : Img_Base(bk){ background = bk; }

    // void drawDot(float radius, Vec2f pos, unsigned color); // TODO: Implement
    // void drawPath(float radius, Vec2f start, Vec2f end, unsigned color); // TODO: Implement

    unsigned background;
};

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){
        timeBillboard.scale({ 1.55F, -0.2F, 1.0F });
        timeBillboard.shift({ 0.0F, -0.844F, 0.0F });
        objectsBillboard.scale({ 3.0F, 0.2F, 1.0F });
        objectsBillboard.shift({ 0.0F, -0.95F, 0.0F });
        actionsBillboard.scale({ 3.0F, -0.2F, 1.0F });
        actionsBillboard.shift({ 0.0F, 0.95F, 0.0F });
        sculptBillboard.scale({ -0.25F, 3.0F, 1.0F });
        sculptBillboard.shift({ -0.935F, 0.0F, 0.0F });
        paintBillboard.scale({ -0.25F, 3.0F, 1.0F });
        paintBillboard.shift({ 0.935F, 0.0F, 0.0F });
        // modeBillboard.rotateAll({ 0.3F, 0.0F, 0.0F });
        modeBillboard.scale({ 0.5F, 0.085F, 1.0F });
        modeBillboard.shift({ 0.0F, 0.88F, 0.0F });
        propsBillboard.scale({ 0.5F, 0.5F, 1.0F });
        propsBillboard.toggleShow(false);
        // propsBillboard.shift({ 0.0F, 0.5F, 0.0F });
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;
    static unsigned short option;

    std::vector<std::pair<Geo_Mesh, Geo_Actor>> objects;

    Geo_Quad2D backdropMesh = Geo_Quad2D(1.5F);
    Geo_Actor backdropActor = Geo_Actor(&backdropMesh);

    Geo_Paneboard modeBillboard = Geo_Paneboard("mode_board");
    Geo_Paneboard timeBillboard = Geo_Paneboard("time_board");
    Geo_Crossboard actionsBillboard = Geo_Crossboard("actions_board", SANDBOX_PANE_COUNT);
    Geo_Billboard objectsBillboard = Geo_Billboard("objects_board", SANDBOX_PANE_COUNT, 2);
    Geo_Listboard sculptBillboard = Geo_Listboard("sculpt_board", SANDBOX_PANE_COUNT);
    Geo_Listboard paintBillboard = Geo_Listboard("paint_board", SANDBOX_PANE_COUNT);
    Geo_Gridboard propsBillboard = Geo_Gridboard("props_board", 3);

    Geo_Orb editorOrb = Geo_Orb(0.025F);
    Geo_Grid editorGrid = Geo_Grid("editor_grid", &editorOrb, Geo_Grid_Params(std::make_pair(SANDBOX_EDITS_COUNT, (1.5F * 2) / (float)SANDBOX_EDITS_COUNT)));
    Geo_Chain editorChain = Geo_Chain("editor_chain", &editorOrb, Vec3f({ 0.0F, (1.5F * 2) / (float)SANDBOX_EDITS_COUNT, 0.0F }), SANDBOX_EDITS_COUNT);
private:
    void updateOverlay();

    Topl_Scene mainScene, editsScene;
    Topl_Scene overlayScene, backdropScene;
    Topl_Camera fixedCamera;

    std::vector<Img_Base*> _images;
    std::vector<Img_Button*> _buttons;
    std::vector<Img_Label*> _labels;
    std::vector<Img_Dial*> _dials;
    std::vector<Img_Slider*> _sliders;
} *_DEMO;