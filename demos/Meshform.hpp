#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MESHFORM_SIZE 0.45
#define MESHFORM_TESS 2
#define MESHFORM_INDEX 0
#define MESHFORM_INC 1.01F
#define MESHFORM_DEC 0.91F
#define MESHFORM_CURVE 0.005F

#define MESHFORM_GRADIENT 0
#define MESHFORM_LINES 1
#define MESHFORM_CHECKER 2

Vec3f rigidTForm(unsigned index, Vec3f target, Vec3f amount){
    return (index % 6 * MESHFORM_TESS == 0)? target * amount : target;
}

Vec3f curveTForm(unsigned index, Vec3f target, Vec3f amount){
    return Vec3f({
        target.data[0] * (1.0F + (sinf(amount.data[0] * index) * MESHFORM_CURVE)),
        target.data[1] * (1.0F + (sinf(amount.data[1] * index) * MESHFORM_CURVE)),
        target.data[2] * (1.0F + (sinf(amount.data[2] * index) * MESHFORM_CURVE))
    });
}

/* Vec3f elongTForm(Vec3f target, Vec3f amount){
    target.data[0] = (target.data[0] > amount.data[0])? target.data[0] * 1.05 + amount.data[0] : target.data[0] * 0.95 - amount.data[0]; 
    target.data[1] = (target.data[1] > amount.data[1])? target.data[1] * 1.05 + amount.data[1] : target.data[1] * 0.95 - amount.data[1]; 
    target.data[2] = (target.data[2] > amount.data[2])? target.data[2] * 1.05 + amount.data[2] : target.data[2] * 0.95 - amount.data[2]; 

    return target;
}

Vec3f trialTForm(Vec3f target, Vec3f amount){
    Vec3f angles = Vec3f({ atan(target[1] / target[0]) - (float)MATH_HALF_PI, atan(target[1] / target[2]) - (float)MATH_HALF_PI, atan(target[0] / target[2]) - (float)MATH_HALF_PI });

    return target * angles; // for testing
} */

struct Meshform_Demo : public Topl_Program {
    Meshform_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshform", backend){
        for(unsigned o = 0; o < 3; o++){
            if(MESHFORM_TESS > 0) trigOrbs[o]->tesselate(MESHFORM_TESS);
            if(MESHFORM_TESS > 0) quadOrbs[o]->tesselate(MESHFORM_TESS);
            if(MESHFORM_TESS > 0) hexOrbs[o]->tesselate(MESHFORM_TESS);
            if(MESHFORM_TESS > 0) decOrbs[o]->tesselate(MESHFORM_TESS);
            if(o > 0) trigOrbs[o]->modify((o % 2 == 1)? curveTForm : rigidTForm, Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC }));
            if(o > 0) quadOrbs[o]->modify((o % 2 == 1)? curveTForm : rigidTForm, Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC }));
            if(o > 0) hexOrbs[o]->modify((o % 2 == 1)? curveTForm : rigidTForm, Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC }));
            if(o > 0) decOrbs[o]->modify((o % 2 == 1)? curveTForm : rigidTForm, Vec3f({ MESHFORM_INC, MESHFORM_INC, MESHFORM_INC }));
        }
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

    Geo_Actor orbActors[3][4] = {
        { trigOrbs[0], quadOrbs[0], hexOrbs[0], decOrbs[0] },
        { trigOrbs[1], quadOrbs[1], hexOrbs[1], decOrbs[1] },
        { trigOrbs[2], quadOrbs[2], hexOrbs[2], decOrbs[2] }
    };
#ifdef RASTERON_H
    Img_Volume volumeImg = Img_Volume(256);
#endif
private:
    void onAnyKey(char key);
#ifdef RASTERON_H
    void genTex3D(unsigned short mode, unsigned color1, unsigned color2);
#endif

    Topl_Scene scene = PROGRAM_SCENE;

    std::thread* textureThread = nullptr;
} *_DEMO;
