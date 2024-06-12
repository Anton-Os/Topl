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

#define SANDBOX_MESH_SCALE 0.3
#define SANDBOX_MESH_COUNT 12

struct Sandbox_Demo : public Topl_Program {
    Sandbox_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Sandbox", backend){
        _surfaces.push_back(new Geo_Quad2D(SANDBOX_MESH_SCALE * 10.0));
        backdropActor = Geo_Actor(_surfaces[0]); // for backdrop

        for(unsigned short s = 0; s < SANDBOX_MESH_COUNT; s++){
            switch(s % 4){
                // case 0: _surfaces.push_back(new Geo_Hex2D(SANDBOX_MESH_SCALE)); break;
                // case 1: _surfaces.push_back(new Geo_Quad2D(SANDBOX_MESH_SCALE)); break;
                // case 2: _surfaces.push_back(new Geo_Circle2D(SANDBOX_MESH_SCALE)); break;
                default: _surfaces.push_back(new Geo_Quad2D(SANDBOX_MESH_SCALE)); break;
            }
            surfaceActors[s] = Geo_Actor(_surfaces[s]);
#ifdef RASTERON_H
            for(unsigned short i = 0; i < SANDBOX_MESH_COUNT; i++) _images.push_back(new Img_Base(std::string(IMAGES_DIR) + "/placeholders/Box-SM.png"));
                // _images.push_back(new Img_Base(0xFF000000 | ((rand() * 10) % 0xFFFFFF)));
#endif
        }

        _billboards.push_back(new Geo_Paneboard("bottom_board")); // Timeline
        _billboards.back()->scale({ 3.85F, -0.3F, 1.0F });
        _billboards.back()->shift({ 0.0F, -0.925F, 0.0F });
        _billboards.push_back(new Geo_Crossboard("top_board", 12)); // Properties
        _billboards.back()->scale({ 3.66F, -0.3F, 1.0F });
        _billboards.back()->shift({ 0.0F, 0.925F, 0.0F });
        _billboards.push_back(new Geo_Listboard("left_board", 20)); // Shapes
        _billboards.back()->scale({ -0.25F, 3.215F, 1.0F });
        _billboards.back()->shift({ -0.95F, 0.0F, 0.0F });
        _billboards.push_back(new Geo_Listboard("right_board", 20)); // Overlays
        _billboards.back()->scale({ -0.25F, 3.215F, 1.0F });
        _billboards.back()->shift({ 0.95F, 0.0F, 0.0F });
    }

    void init() override;
    void loop(double frameTime) override;

    Topl_Camera camera, fixedCamera;
    Topl_Scene mainScene, overlayScene, backdropScene;

    Geo_Actor backdropActor;
    Geo_Actor surfaceActors[SANDBOX_MESH_COUNT];
    // Geo_Actor trigActors[SANDBOX_MESH_COUNT], quadActors[SANDBOX_MESH_COUNT], circleActors[SANDBOX_MESH_COUNT];
protected:
    std::vector<Geo_Mesh*> _meshes;
    std::vector<Geo_Surface*> _surfaces;
    std::vector<Geo_Cone*> _cones;
    std::vector<Geo_Volume*> _volumes;
    std::vector<Geo_Orb*> _orbs;
    std::vector<Geo_Iter*> _iterables;

    std::vector<Geo_Actor> _actors;
#ifdef TOPL_ENABLE_PHYSICS
    std::vector<Phys_Actor> _physics; // std::vector<Phys_Connector> _connectors;
#endif
#ifdef RASTERON_H
    std::vector<Img_Base*> _images;
    // std::vector<Img_Array> _imgArrays; 
    // std::vector<Img_Volume> _imgVolumes; 
#endif
    std::vector<Geo_Grid*> _grids;
    std::vector<Geo_Chain*> _chains;
    std::vector<Geo_Billboard*> _billboards;
#ifdef RASTERON_H
    std::vector<Img_Button*> _buttons; 
    std::vector<Img_Label*> _labels; 
    std::vector<Img_Dial*> _dials; 
    std::vector<Img_Slider*> _sliders;
#endif
#ifdef TOPL_ENABLE_MODELS
    // Geo_Model> _models;
#endif
} *_DEMO;