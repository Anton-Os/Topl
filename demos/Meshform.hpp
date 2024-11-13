#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MESHFORM_SIZE 1.5
#define MESHFORM_TESS 8

struct Meshform_Demo : public Topl_Program {
    Meshform_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshform", backend){
        trigOrb->tesselate(MESHFORM_TESS);
        trigOrb->drawMode = DRAW_Triangles;
        quadOrb->tesselate(MESHFORM_TESS);
        quadOrb->drawMode = DRAW_Fan;
        hexOrb->tesselate(MESHFORM_TESS);
        hexOrb->drawMode = DRAW_Strip;
        decOrb->tesselate(MESHFORM_TESS);
        decOrb->drawMode = DRAW_Lines;
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