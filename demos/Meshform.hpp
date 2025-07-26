#include "Geo_Construct.hpp"
#include "meshes/Geo_Orboid.hpp"
#include "meshes/Geo_Fractal.hpp"

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

Vec3f rigidTForm(Vec3f target, Vec3f amount);
Vec3f curveTForm(Vec3f target, Vec3f amount);
Vec3f pullTForm(Vec3f target, Vec3f amount);

Geo_Vertex vertexTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation);
Geo_Vertex distanceTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation);
Geo_Vertex midpointTform(const Geo_Vertex& vertex, const Geo_Vertex& midpoint, unsigned primID, unsigned invocation);

struct Meshform_Demo : public Topl_Program {
    Meshform_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshform", backend){}

    void init() override;
    void preloop() override {
        Topl_Program::preloop();
#ifdef RASTERON_H
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
    Geo_Fractal* fractals[4] = {
        new Geo_Fractal(ShapeFractal({ MESHFORM_SIZE, 12, 12, 12 })), new Geo_Fractal(ShapeFractal({ MESHFORM_SIZE, 24, 24, 24 })),
        new Geo_Fractal(ShapeFractal({ MESHFORM_SIZE, 120, 120, 120 })), new Geo_Fractal(ShapeFractal({ MESHFORM_SIZE, 240, 240, 240 }))
    };
    Geo_Torus* torus = new Geo_Torus(0.5F, Shape3D({ 0.25F, 20, 20 }));

    Geo_Actor orbActors[3][4] = {
        { orbs[0][0], orbs[1][0], orbs[2][0], orbs[3][0] },
        { orbs[0][1], orbs[1][1], orbs[2][1], orbs[3][1] },
        { orbs[0][2], orbs[1][2], orbs[2][2], orbs[3][2] }
    };
    Geo_Actor fractalActors[4] = { Geo_Actor(fractals[0]), Geo_Actor(fractals[1]), Geo_Actor(fractals[2]), Geo_Actor(fractals[3]) };
    Geo_Actor torusActor = Geo_Actor(torus);
#ifdef RASTERON_H
    Sampler_3D volumeImg = Sampler_3D(256);
#endif
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;
#ifdef RASTERON_H
    void genTex3D(unsigned short mode, unsigned color1, unsigned color2);
#endif
    void genShapes(std::pair<vTformCallback, Vec3f> transform1, std::pair<vTformCallback, Vec3f> transform2);
    void rebuildGeometry(std::thread* thread);

    Topl_Scene scene = PROGRAM_SCENE;

    std::thread* textureThread = nullptr;
    std::thread* geometryThread = nullptr;
} *Meshform;
