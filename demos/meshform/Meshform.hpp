#include "Geo_Construct.hpp"
#include "meshes/Geo_Orboid.hpp"
#include "meshes/Geo_Freeform.hpp"

#include "program/Topl_Program.hpp"

#define MESHFORM_SIZE 0.5
#define MESHFORM_TESS 0
#define MESHFORM_INDEX 0
#define MESHFORM_AMOUNT 0.25F
#define MESHFORM_INC 1.15F // 1.01
#define MESHFORM_DEC 0.9F // 0.99
#define MESHFORM_CURVE 0.1F // 0.005F

#define MESHFORM_GRADIENT 0
#define MESHFORM_LINES 1
#define MESHFORM_CHECKER 2
#define MESHFORM_NOISE 3

#include "Transforms.h"

struct Meshform_Demo : public Topl_Program {
    Meshform_Demo(const char* execPath) : Topl_Program(execPath, "Meshform"){}
    Meshform_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshform", backend){}

    void init() override;
    void preloop() override {
        Topl_Program::preloop();
#ifdef TOPL_ENABLE_TEXTURES
        colorPicker(&scene);
#endif
    }
    void loop(double frameTime) override;

    Geo_Orb* orbs[4][3] = {
        { new Geo_TrigOrb(MESHFORM_SIZE), new Geo_TrigOrb(MESHFORM_SIZE), new Geo_TrigOrb(MESHFORM_SIZE) },
        { new Geo_QuadOrb(MESHFORM_SIZE), new Geo_QuadOrb(MESHFORM_SIZE), new Geo_QuadOrb(MESHFORM_SIZE) },
        { new Geo_HexOrb(MESHFORM_SIZE), new Geo_HexOrb(MESHFORM_SIZE), new Geo_HexOrb(MESHFORM_SIZE) },
        { new Geo_DecOrb(MESHFORM_SIZE), new Geo_DecOrb(MESHFORM_SIZE), new Geo_DecOrb(MESHFORM_SIZE) }
    };
    Geo_Freeform* freeforms[4] = {
        new Geo_Freeform(ShapeFreeform({ MESHFORM_SIZE / 2.0F, 5, 5, 5 })), new Geo_Freeform(ShapeFreeform({ MESHFORM_SIZE / 2.0F, 10, 10, 10 })),
        new Geo_Freeform(ShapeFreeform({ MESHFORM_SIZE / 2.0F, 15, 15, 15 })), new Geo_Freeform(ShapeFreeform({ MESHFORM_SIZE / 2.0F, 15, 10, 5 }))
    };
    Geo_Torus* torus = new Geo_Torus(0.5F, Shape3D({ 0.25F, 20, 20 }));

    Geo_Actor orbActors[3][4] = {
        { orbs[0][0], orbs[1][0], orbs[2][0], orbs[3][0] },
        { orbs[0][1], orbs[1][1], orbs[2][1], orbs[3][1] },
        { orbs[0][2], orbs[1][2], orbs[2][2], orbs[3][2] }
    };
    Geo_Actor freeformActors[4] = { Geo_Actor(freeforms[0]), Geo_Actor(freeforms[1]), Geo_Actor(freeforms[2]), Geo_Actor(freeforms[3]) };
    Geo_Actor torusActor = Geo_Actor(torus);
#ifdef TOPL_ENABLE_TEXTURES
    Sampler_3D volumeImg = Sampler_3D(256);
#endif
private:
    void onAnyKey(keyboard_t key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;
#ifdef TOPL_ENABLE_TEXTURES
    void genTex3D(unsigned short mode, unsigned color1, unsigned color2);
#endif
    void genShapes(std::pair<vTformCallback, Vec3f> transform1, std::pair<vTformCallback, Vec3f> transform2);

    Topl_Scene scene = PROGRAM_SCENE;
} *Meshform;
