#include "program/Topl_Program.hpp"

#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Billboards.hpp"

#define SANDBOX_ACTION 0
#define SANDBOX_TIME 1
#define SANDBOX_SCULPT 2
#define SANDBOX_PAINT 3

#define SANDBOX_MOVE 0
#define SANDBOX_ROTATE 1
#define SANDBOX_SIZE 2

#define SANDBOX_CANVAS 1.5F
#define SANDBOX_PANES 12
#define SANDBOX_EDIT 5
#define SANDBOX_SEQUENCE 60.0

struct Sampler_Canvas : public Sampler_Color {
    Sampler_Canvas(unsigned bk) : Sampler_Color(bk){ 
        tag = &refreshTag;
        background = bk;
    }

    void refresh(){
        char tag[32];
        sprintf(tag, "%s-%d", "canvas", rand() % 9999); // randomize image tag
        refreshTag = tag;
    }

    void setBackground(unsigned bk){
        for(unsigned p = 0; p < image->height * image->width; p++)
            if(*(image->data + p) == background) *(image->data + p) = bk;
        background = bk;
        for(unsigned p = 0; p < image->height * image->width; p++) *(image->data + p) = background;
        refresh();
    }

    void drawDot(float radius, Vec2f pos, unsigned color){ // TODO: Handle path and object draw cases
        for(unsigned p = 0; p < image->height * image->width; p++){
            double x = (1.0 / (double)image->width) * (p % image->width);
            double y = (1.0 / (double)image->height) * (p / image->width);

            double dist = sqrt(pow(x - pos.data[0], 2.0) + pow(y - pos.data[1], 2.0));
            if(dist < radius) *(image->data + p) = colors_blend(color, *(image->data + p), (*(image->data + p) != background)? blend : 0.0F);
        }
        refresh();
    }

    void drawPath(float radius, Vec2f pos1, Vec2f pos2, unsigned color){
        for(unsigned p = 0; p < image->height * image->width; p++){
            double x = (1.0 / (double)image->width) * (p % image->width);
            double y = (1.0 / (double)image->height) * (p / image->width);

            float dist = Vec2f(pos2 - pos1).len();
            double xDiff = (x - pos2[0]) / (pos1[0] - pos2[0]); double yDiff = (y - pos2[1]) / (pos1[1] - pos2[1]);
            double xRang = fabs(x - pos2[0]) + fabs(x - pos1[0]); double yRang = fabs(y - pos2[1]) + fabs(y - pos1[1]);
            Vec2f midpoint = (pos1 + pos2) * 0.5F;

            // if(xDiff > yDiff - (radius * (1.0 / dist) * (xDiff + yDiff)) && xDiff < yDiff + (radius * (1.0 / dist) * (xDiff + yDiff))) 
            if(xDiff > yDiff - (radius * (radius / (xRang * yRang))) && xDiff < yDiff + (radius * (radius / (xRang * yRang))) && Vec2f({ (float)x - midpoint[0], (float)y - midpoint[1] }).len() < dist * 0.5F)
                *(image->data + p) = colors_blend(color, *(image->data + p), (*(image->data + p) != background)? blend : 0.0F);
        }
        refresh();
    }

    void drawBox(Vec2f pos1, Vec2f pos2, unsigned color){
        for(unsigned p = 0; p < image->height * image->width; p++){
            double x = (1.0 / (double)image->width) * (p % image->width);
            double y = (1.0 / (double)image->height) * (p / image->width);

            float xRanges[2] = { (pos1[0] > pos2[0])? pos2[0] : pos1[0], (pos1[0] > pos2[0])? pos1[0] : pos2[0] };
            float yRanges[2] = { (pos1[1] > pos2[1])? pos2[1] : pos1[1], (pos1[1] > pos2[1])? pos1[1] : pos2[1] };

            if(x > xRanges[0] && x < xRanges[1] && y > yRanges[0] && y < yRanges[1])
                *(image->data + p) = colors_blend(color, *(image->data + p), (*(image->data + p) != background)? blend : 0.0F);
        }
    }

    char* refreshTag = "canvas";
    float blend = 0.5F;
    unsigned background;
};

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){
        timeBillboard.scale({ 1.55F, -0.2F, 1.0F });
        timeBillboard.shift({ 0.0F, -0.95F, 0.0F });
        timeBillboard.getGeoActor(1)->updateSize({ 0.25F, 0.0F, 0.0F });
        timeTextBillboard.scale({ 0.5F, 0.2F, 1.0F });
        timeTextBillboard.shift({ 0.6F, -0.95F, 0.01F });
        timeTextBillboard.getGeoActor(0)->updateSize({ 0.0F, -0.1F, 0.0F });
        timeCtrlBillboard.scale({ 0.5F, -0.2F, 1.0F });
        timeCtrlBillboard.shift({ -0.6F, -0.95F, 0.01F });
        objectsBillboard.scale({ 1.5F, 0.1F, 1.0F });
        objectsBillboard.shift({ 0.0F, -0.95F, 0.0F });
        objectsBillboard.toggleShow(false);
        actionsBillboard.scale({ 1.5F, -0.13F, 1.0F });
        actionsBillboard.shift({ 0.0F, 0.975F, 0.0F });
        // actionsBillboard.getImgRoot()->setImage(cornerImgOp(actionsBillboard.getImgRoot()->getImage(), 0.0, 0.0, 1.5, 1.5));
        // for(unsigned b = 0; b < actionsBillboard.getActorCount(); b++) actionsBillboard.getImgAt(b)->setImage(cornerImgOp(actionsBillboard.getImgAt(b)->getImage(), 0.0, 0.0, 0.1, 0.1));
        sculptBillboard.scale({ -0.12F, 1.15F, 1.0F });
        sculptBillboard.shift({ -0.965F, 0.0F, 0.0F });
        // sculptBillboard.getImgRoot()->setImage(cornerImgOp(sculptBillboard.getImgRoot()->getImage(), 1.5, 0.0, 1.5, 0.0));
        // for(unsigned b = 0; b < sculptBillboard.getActorCount(); b++) sculptBillboard.getImgAt(b)->setImage(cornerImgOp(sculptBillboard.getImgAt(b)->getImage(), 0.5, 0.0, 0.5, 0.0));
        paintBillboard.scale({ -0.12F, 1.15F, 1.0F });
        paintBillboard.shift({ 0.965F, 0.0F, 0.0F });
        // paintBillboard.getImgRoot()->setImage(cornerImgOp(paintBillboard.getImgRoot()->getImage(), 0.0, 1.5, 0.0, 1.5));
        // for(unsigned b = 0; b < paintBillboard.getActorCount(); b++) paintBillboard.getImgAt(b)->setImage(cornerImgOp(paintBillboard.getImgAt(b)->getImage(), 0.0, 0.5, 0.0, 0.5));
        // modeBillboard.rotateAll({ 0.3F, 0.0F, 0.0F });
        modeBillboard.scale({ 0.4F, 0.06F, 1.0F });
        modeBillboard.shift({ 0.0F, 0.975F, 0.0F });
        propsBillboard.scale({ 0.5F, 0.5F, 1.0F });
        propsBillboard.toggleShow(false);
        // propsBillboard.shift({ 0.0F, 0.5F, 0.0F });

        isEnable_background = false;
        isEnable_overlays = false;
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short option;

    std::vector<Geo_Mesh*> objectMeshes;
    std::vector<Sampler_2D*> objectTextures;
    std::vector<Phys_Actor*> objectPhysics;
    std::vector<Geo_Actor*> objectActors;

    Geo_Quad2D backdropMesh = Geo_Quad2D(1.0F);
    Geo_Actor backdropActor = Geo_Actor(&backdropMesh);

    Geo_Paneboard modeBillboard = Geo_Paneboard("mode_board");
    Geo_Paneboard timeBillboard = Geo_Paneboard("time_board");
    Geo_Paneboard timeTextBillboard = Geo_Paneboard("time_text_board");
    Geo_Crossboard timeCtrlBillboard = Geo_Crossboard("time_ctrl_board", 3);
    Geo_Crossboard actionsBillboard = Geo_Crossboard("actions_board", SANDBOX_PANES);
    Geo_Billboard objectsBillboard = Geo_Billboard("objects_board", SANDBOX_PANES, 2);
    Geo_Listboard sculptBillboard = Geo_Listboard("sculpt_board", SANDBOX_PANES);
    Geo_Listboard paintBillboard = Geo_Listboard("paint_board", SANDBOX_PANES);
    Geo_Gridboard propsBillboard = Geo_Gridboard("props_board", 3);

    Geo_Orb plotOrb = Geo_Orb(0.025F);
    Geo_Grid plotGrid = Geo_Grid("plot_grid", &plotOrb, Geo_Grid_Params(
        std::make_pair(SANDBOX_EDIT, SANDBOX_CANVAS / (float)SANDBOX_EDIT), 
        std::make_pair(SANDBOX_EDIT, SANDBOX_CANVAS / (float)SANDBOX_EDIT)
    ));
    Geo_Chain plotChain = Geo_Chain("plot_chain", &plotOrb, Vec3f({ 0.0F, (SANDBOX_CANVAS * 2) / (float)SANDBOX_EDIT, 0.0F }), SANDBOX_EDIT);
private:
    void updateOverlay();

    std::thread overlayThread;

    Topl_Scene mainScene, editsScene;
    Topl_Scene overlayScene, canvasScene;
    Topl_Camera fixedCamera;

    void onAnyKey(char key);
    void onTimePanePress(MOUSE_Event event, Geo_Actor* actor);
    void onActionPanePress(MOUSE_Event event, Geo_Actor* actor);
    void onSculptPanePress(MOUSE_Event event, Geo_Actor* actor);
    void onPaintPanePress(MOUSE_Event event, Geo_Actor* actor);

    std::vector<Sampler_2D*> _images;
    std::vector<Sampler_Button*> _buttons;
    std::vector<Sampler_Label*> _labels;
    std::vector<Sampler_Dial*> _dials;
    std::vector<Sampler_Slider*> _sliders;
} *_DEMO;