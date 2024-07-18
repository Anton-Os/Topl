#include "program/Topl_Program.hpp"

#include "constructs/Geo_Billboards.hpp"

#define PLAYGROUND_ACTION_MENU 1
#define PLAYGROUND_OBJECT_MENU 2
#define PLAYGROUND_SCULPT_MENU 3
#define PLAYGROUND_PAINT_MENU 4

#define PLAYGROUND_PANE_COUNT 12

struct Playground_Demo : public Topl_Program {
    Playground_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Playground", backend){
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

    Geo_Quad2D backdropMesh = Geo_Quad2D(1.5F);
    Geo_Actor backdropActor = Geo_Actor(&backdropMesh);

    Geo_Paneboard modeBillboard = Geo_Paneboard("mode_board");
    Geo_Paneboard timeBillboard = Geo_Paneboard("time_board");
    Geo_Crossboard actionsBillboard = Geo_Crossboard("actions_board", PLAYGROUND_PANE_COUNT);
    Geo_Billboard objectsBillboard = Geo_Billboard("objects_board", PLAYGROUND_PANE_COUNT, 2);
    Geo_Listboard sculptBillboard = Geo_Listboard("sculpt_board", PLAYGROUND_PANE_COUNT);
    Geo_Listboard paintBillboard = Geo_Listboard("paint_board", PLAYGROUND_PANE_COUNT);
    Geo_Gridboard propsBillboard = Geo_Gridboard("props_board", 3);
private:
    void updateOverlay();

    Topl_Scene overlayScene, backdropScene;
    Topl_Camera fixedCamera;

    std::vector<Img_Base*> _images;
    std::vector<Img_Button*> _buttons;
    std::vector<Img_Label*> _labels;
    std::vector<Img_Dial*> _dials;
    std::vector<Img_Slider*> _sliders;
} *_DEMO;