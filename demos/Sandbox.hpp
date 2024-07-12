#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Orb.hpp"
#include "meshes/Geo_Iterable.hpp"

#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"
#include "constructs/Geo_Billboards.hpp"

#include "physics/Phys_Actor.h"
#include "physics/Phys_Connector.h"

#include "program/Topl_Program.hpp"

#define SANDBOX_MESH_SCALE 0.1
#define SANDBOX_MESH_COUNT 10
#define SANDBOX_PANE_COUNT 12

#define SANDBOX_PICKER 1
#define SANDBOX_ACTION 2
#define SANDBOX_SCULPT 3
#define SANDBOX_PAINT 4

enum SANDBOX_Action {
    SANDBOX_Move = 1, SANDBOX_Rotate = 2, SANDBOX_Scale = 3,
    SANDBOX_Zoom = SANDBOX_PANE_COUNT, SANDBOX_Pivot = SANDBOX_PANE_COUNT - 1, SANDBOX_Pan = SANDBOX_PANE_COUNT - 2,
    SANDBOX_Sculpt = -1, SANDBOX_Paint = -2
};

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){
        _surfaces.push_back(new Geo_Quad2D(1000.0));
        backdropActor = Geo_Actor(_surfaces[0]); // for backdrop

        for(unsigned short s = 0; s < SANDBOX_MESH_COUNT; s++){
            switch(s % 4){
                case 0: _surfaces.push_back(new Geo_Hex2D(SANDBOX_MESH_SCALE)); _cones.push_back(new Geo_HexCone(SANDBOX_MESH_SCALE)); _volumes.push_back(new Geo_Hex3D(SANDBOX_MESH_SCALE)); break;
                case 1: _surfaces.push_back(new Geo_Trig2D(SANDBOX_MESH_SCALE)); _cones.push_back(new Geo_TrigCone(SANDBOX_MESH_SCALE));_volumes.push_back(new Geo_Trig3D(SANDBOX_MESH_SCALE)); break;
                case 2: _surfaces.push_back(new Geo_Circle2D(SANDBOX_MESH_SCALE)); _cones.push_back(new Geo_CircleCone(SANDBOX_MESH_SCALE)); _volumes.push_back(new Geo_Circle3D(SANDBOX_MESH_SCALE)); break;
                default: _surfaces.push_back(new Geo_Quad2D(SANDBOX_MESH_SCALE)); _cones.push_back(new Geo_QuadCone(SANDBOX_MESH_SCALE)); _volumes.push_back(new Geo_Quad3D(SANDBOX_MESH_SCALE)); break;
            }
            _orbs.push_back(new Geo_Orb(Volume({ (float)(SANDBOX_MESH_SCALE / SANDBOX_MESH_COUNT) * (s + 1) })));
            // for(unsigned v = 3; v < s + 2; v++) _meshes.push_back(new Geo_Mesh(/* number of random vertices matching the original */)); // TODO: create meshes with randomized vertices
            // _iterables.push_back(new Geo_Iter(_meshes[s])); // TODO: create iterables based on previous meshes

            surfaceActors[s] = Geo_Actor(_surfaces[s]);
            coneActors[s] = Geo_Actor(_cones[s]);
            volumeActors[s] = Geo_Actor(_volumes[s]);
        }
#ifdef RASTERON_H
        // for(unsigned short i = 0; i < SANDBOX_MESH_COUNT; i++) _images.push_back(new Img_Base((i % 3 == 0)? 0xAAFF0000 : (i % 3 == 1)? 0xAA00FF00 : 0xAA0000FF));
        for(unsigned short i = 0; i < SANDBOX_MESH_COUNT; i++){ 
            _images.push_back(new Img_Base(RAND_COLOR()));
            std::string fontPath = std::string(FONTS_DIR) + "CutiveMono-Regular.ttf";
            std::string labelText = std::to_string(i);
            // Rasteron_Text textObjRasteron_Text({ fontPath.c_str(), labelText.c_str(), 0xFF333333, 0xFF00FF00 })
            // _imgTexts.push_back(new Img_Base());
        }
        for(unsigned short i = 0; i < 8; i++){ // unbound textures
            _images.push_back(new Img_Base(noiseImgOp_tiled({1024, 1024}, { (unsigned)pow(2, i + 1), (unsigned)pow(2, i + 1), RAND_COLOR(), RAND_COLOR() })));
            mainScene.addTexture(std::to_string(i + 1), _images.back());
        }
        _imgVolumes.push_back(new Img_Volume(16)); _imgVolumes.push_back(new Img_Volume(32)); _imgVolumes.push_back(new Img_Volume(64));
        // _images.push_back(new Img_Base(std::string(IMAGES_DIR) + "/placeholders/Box-SM.png"));
#endif

        _billboards.push_back(new Geo_Paneboard("sequence_board"));
        _billboards.back()->scale({ 2.0F, -0.2F, 1.0F });
        _billboards.back()->shift({ 0.0F, -0.844F, 0.0F });
        _billboards.push_back(new Geo_Crossboard("objs_board", SANDBOX_PANE_COUNT));
        _billboards.back()->scale({ 3.66F, -0.2F, 1.0F });
        _billboards.back()->shift({ 0.0F, -0.95F, 0.0F });
        _billboards.push_back(new Geo_Crossboard("actions_board", SANDBOX_PANE_COUNT));
        _billboards.back()->scale({ 3.66F, -0.2F, 1.0F });
        _billboards.back()->shift({ 0.0F, 0.95F, 0.0F });
        _billboards.push_back(new Geo_Listboard("sculpt_board", SANDBOX_PANE_COUNT));
        _billboards.back()->scale({ -0.25F, 3.0F, 1.0F });
        _billboards.back()->shift({ -0.935F, 0.0F, 0.0F });
        _billboards.push_back(new Geo_Listboard("paint_board", SANDBOX_PANE_COUNT));
        _billboards.back()->scale({ -0.25F, 3.0F, 1.0F });
        _billboards.back()->shift({ 0.935F, 0.0F, 0.0F });
        _billboards.push_back(new Geo_Paneboard("mode_board"));
        _billboards.back()->scale({ 0.85F, -0.15F, 1.0F });
        _billboards.back()->shift({ 0.0F, 0.867F, 0.0F });
        _billboards.push_back(new Geo_Billboard("props_board", 2, 3));
        _billboards.back()->scale({ 0.66F, 0.45F, 1.0F });
        _billboards.back()->shift({ 0.0F, 0.5F, 0.0F });
#ifdef RASTERON_H
        for(unsigned short l = 0; l < 20; l++){
            // _labels.push_back(new Img_Label(MENU_Medium, Rasteron_Text({ fontPath.c_str(), labelText.c_str(), 0xFF333333, 0xFF00FF00 })));
            _buttons.push_back(new Img_Button(MENU_Small));
            _dials.push_back(new Img_Dial(MENU_Small, 2));
            _sliders.push_back(new Img_Slider(MENU_Small, 2));
        }
#endif
    }

    void init() override;
    void loop(double frameTime) override;

    Topl_Camera camera, fixedCamera;
    Topl_Scene mainScene, overlayScene, backdropScene;

    Geo_Actor backdropActor;
    Geo_Actor surfaceActors[SANDBOX_MESH_COUNT], coneActors[SANDBOX_MESH_COUNT], volumeActors[SANDBOX_MESH_COUNT];
    // Geo_Actor trigActors[SANDBOX_MESH_COUNT], quadActors[SANDBOX_MESH_COUNT], circleActors[SANDBOX_MESH_COUNT];
// protected:
    static void onBackdropPick(MOUSE_Event event);
    static void onObjectPick(MOUSE_Event event);
    static void onMenuPick(MOUSE_Event event);

    static SANDBOX_Action _action;
    static unsigned short _mode;

    std::vector<Geo_Mesh*> _meshes;
    std::vector<Geo_Surface*> _surfaces;
    std::vector<Geo_Cone*> _cones;
    std::vector<Geo_Volume*> _volumes;
    std::vector<Geo_Orb*> _orbs;
    std::vector<Geo_Iter*> _iterables;

    std::vector<Geo_Actor*> _actors;
#ifdef TOPL_ENABLE_PHYSICS
    std::vector<Phys_Actor*> _physics; // std::vector<Phys_Connector> _connectors;
#endif
#ifdef RASTERON_H
    std::vector<Img_Base*> _images;
    std::vector<Img_Base*> _imgTexts; // std::vector<Img_Txt*> _imgTexts;
    std::vector<Img_Volume*> _imgVolumes; 
#endif
    std::vector<Geo_Grid*> _grids;
    std::vector<Geo_Chain*> _chains;
    std::vector<Geo_Billboard*> _billboards;
#ifdef RASTERON_H
    std::vector<Img_Button*> _buttons;
    // std::vector<Img_Label*> _labels;
    std::vector<Img_Dial*> _dials; 
    std::vector<Img_Slider*> _sliders;
#endif
#ifdef TOPL_ENABLE_MODELS
    // std::vector<Geo_Model> _models;
#endif
} *_DEMO;