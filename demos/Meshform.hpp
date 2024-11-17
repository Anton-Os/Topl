#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MESHFORM_SIZE 1.0
#define MESHFORM_TESS 8

Vec3f spikeTForm(Vec3f target, Vec3f amount){
    static unsigned svCount = 0;

    if(target.data[0] == 0.0 && target.data[1] == 0.0 && target.data[2] == 0.0) svCount = 0; // reset at origin
    else svCount++;

    return (svCount % MESHFORM_TESS == 1)? target * amount : target;
}

Vec3f waveTForm(Vec3f target, Vec3f amount){
    static unsigned svCount = 0;

    if(target.data[0] == 0.0 && target.data[1] == 0.0 && target.data[2] == 0.0) svCount = 0; // reset at origin
    else svCount++;

    return Vec3f({ target.data[0] * (1.0F + sinf(amount.data[0] * svCount) * 0.25F), target.data[1] * (1.0F + sinf(amount.data[1] * svCount) * 0.25F), target.data[2] * (1.0F + sinf(amount.data[2] * svCount) * 0.25F) });
}

Vec3f elongTForm(Vec3f target, Vec3f amount){
    target.data[0] = (target.data[0] > amount.data[0])? target.data[0] * 1.05 + amount.data[0] : target.data[0] * 0.95 - amount.data[0]; 
    target.data[1] = (target.data[1] > amount.data[1])? target.data[1] * 1.05 + amount.data[1] : target.data[1] * 0.95 - amount.data[1]; 
    target.data[2] = (target.data[2] > amount.data[2])? target.data[2] * 1.05 + amount.data[2] : target.data[2] * 0.95 - amount.data[2]; 

    return target;
}

struct Meshform_Demo : public Topl_Program {
    Meshform_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshform", backend){
        trigOrb->tesselate(MESHFORM_TESS);
        trigOrb->drawMode = DRAW_Triangles;
        quadOrb->tesselate(MESHFORM_TESS);
        // quadOrb->drawMode = DRAW_Fan;
        quadOrb->modify(spikeTForm, Vec3f({ 1.05F, 1.05F, 1.05F }));
        hexOrb->tesselate(MESHFORM_TESS);
        // hexOrb->drawMode = DRAW_Strip;
        hexOrb->modify(waveTForm, Vec3f({ 0.01F, 0.01F, 0.01F }));
        decOrb->tesselate(MESHFORM_TESS);
        // decOrb->drawMode = DRAW_Lines;
        decOrb->modify(elongTForm, Vec3f({ 0.0F, 0.0F, 0.0F }));
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_TrigOrb* trigOrb = new Geo_TrigOrb(MESHFORM_SIZE);
    Geo_QuadOrb* quadOrb = new Geo_QuadOrb(MESHFORM_SIZE);
    Geo_HexOrb* hexOrb = new Geo_HexOrb(MESHFORM_SIZE);
    Geo_DecOrb* decOrb = new Geo_DecOrb(MESHFORM_SIZE);

    Geo_Actor orbActors[4] = { trigOrb, quadOrb, hexOrb, decOrb };
#ifdef RASTERON_H
    Img_Volume volumeImg = Img_Volume(256);
#endif
private:
    Topl_Scene scene;
} *_DEMO;