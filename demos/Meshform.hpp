#include "Geo_Construct.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "program/Topl_Program.hpp"

#define MESHFORM_SIZE 0.45
#define MESHFORM_TESS 0
#define MESHFORM_INDEX 0
#define MESHFORM_INC 1.15F // 1.01
#define MESHFORM_DEC 0.9F // 0.99
#define MESHFORM_CURVE 0.1F // 0.005F

#define MESHFORM_GRADIENT 0
#define MESHFORM_LINES 1
#define MESHFORM_CHECKER 2
#define MESHFORM_NOISE 3

Vec3f rigidTForm(Vec3f target, Vec3f amount){
    static unsigned index = 0; 
    index++;
    return (index % 6 * (MESHFORM_TESS + 1) == 0)? target * amount : target;
}

Vec3f curveTForm(Vec3f target, Vec3f amount){
    static unsigned index = 0; 
    index++;

    return Vec3f({
        target.data[0] * (1.0F + (sinf(fabs(1.0F - amount.data[0]) * index) * MESHFORM_CURVE)),
        target.data[1] * (1.0F + (sinf(fabs(1.0F - amount.data[1]) * index) * MESHFORM_CURVE)),
        target.data[2] * (1.0F + (sinf(fabs(1.0F - amount.data[2]) * index) * MESHFORM_CURVE))
    });
}

Vec3f pullTForm(Vec3f target, Vec3f amount){
    Vec3f diffVec = amount - target;

    return (diffVec.len() < MESHFORM_SIZE * 3)? target + (amount * 0.33) : target;
}

struct Meshform_Demo : public Topl_Program {
    Meshform_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshform", backend){
        Vec3f incVec = Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC });
        Vec3f decVec = Vec3f({ MESHFORM_DEC, MESHFORM_DEC, MESHFORM_DEC });
        genShapes(MESHFORM_TESS, std::make_pair(curveTForm, decVec), std::make_pair(rigidTForm, incVec));
    }

    void init() override;
    void preloop() override {
        Topl_Program::preloop();
        colorPicker(&scene);
    }
    void loop(double frameTime) override;

    Geo_TrigOrb* trigOrbs[3] = { new Geo_TrigOrb(MESHFORM_SIZE), new Geo_TrigOrb(MESHFORM_SIZE), new Geo_TrigOrb(MESHFORM_SIZE) };
    Geo_QuadOrb* quadOrbs[3] = { new Geo_QuadOrb(MESHFORM_SIZE), new Geo_QuadOrb(MESHFORM_SIZE), new Geo_QuadOrb(MESHFORM_SIZE) };
    Geo_HexOrb* hexOrbs[3] = { new Geo_HexOrb(MESHFORM_SIZE), new Geo_HexOrb(MESHFORM_SIZE), new Geo_HexOrb(MESHFORM_SIZE) };
    Geo_DecOrb* decOrbs[3] = { new Geo_DecOrb(MESHFORM_SIZE), new Geo_DecOrb(MESHFORM_SIZE), new Geo_DecOrb(MESHFORM_SIZE) };
    Geo_Torus* torus = new Geo_Torus(0.5F, Shape3D({ 0.25F, 20, 20 }));

    Geo_Actor orbActors[3][4] = {
        { trigOrbs[0], quadOrbs[0], hexOrbs[0], decOrbs[0] },
        { trigOrbs[1], quadOrbs[1], hexOrbs[1], decOrbs[1] },
        { trigOrbs[2], quadOrbs[2], hexOrbs[2], decOrbs[2] }
    };
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
    void genShapes(unsigned tessCount, std::pair<vTformCallback, Vec3f> transform1, std::pair<vTformCallback, Vec3f> transform2);

    void renderRecursive(Geo_Actor* actor, TFORM_Type type, unsigned short count);

    Topl_Scene scene = PROGRAM_SCENE;

    std::thread* textureThread = nullptr;
    std::thread* geometryThread = nullptr;
} *_DEMO;
