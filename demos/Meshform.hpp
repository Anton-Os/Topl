#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MESHFORM_SIZE 0.45
#define MESHFORM_TESS 1
#define MESHFORM_INDEX 1

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
        if(MESHFORM_TESS > 0)
            for(unsigned o = 0; o < 3; o++){
                trigOrbs[o]->tesselate(MESHFORM_TESS);
                quadOrbs[o]->tesselate(MESHFORM_TESS);
                hexOrbs[o]->tesselate(MESHFORM_TESS);
                decOrbs[o]->tesselate(MESHFORM_TESS);
            }

        trigOrbs[1]->modify(spikeTForm, Vec3f({ 1.5F, 1.5F, 1.5F }));
        trigOrbs[2]->modify(waveTForm, Vec3f({ 0.0F, 0.0F, 0.0F }));
        quadOrbs[1]->modify(spikeTForm, Vec3f({ 1.5F, 0.0F, 0.0F }));
        quadOrbs[2]->modify(waveTForm, Vec3f({ 1.0F, 0.0F, 0.0F }));
        hexOrbs[1]->modify(spikeTForm, Vec3f({ 0.0F, 1.5F, 0.0F }));
        hexOrbs[2]->modify(waveTForm, Vec3f({ 0.0F, 1.0F, 0.0F }));
        decOrbs[1]->modify(spikeTForm, Vec3f({ 0.0F, 0.0F, 1.5F }));
        decOrbs[2]->modify(waveTForm, Vec3f({ 0.0F, 0.0F, 1.0F }));
    }

    void init() override;
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
    Topl_Scene scene;
} *_DEMO;