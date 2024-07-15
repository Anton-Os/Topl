#include "Geo_Construct.hpp"
#include "meshes/Geo_Surface.hpp"

#include "program/Topl_Program.hpp"

#define MATHART_SLICES 256

struct MathArt_Construct : public Geo_Construct {
    MathArt_Construct() : Geo_Construct("Top"){
        for(unsigned s = 0; s < MATHART_SLICES; s++){
            circleMeshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, CIRCLE_SEGMENTS + ((s * 64) % CIRCLE_SEGMENTS)});
            _geoActors.push_back(Geo_Actor(circleMeshes[s]));
            _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / MATHART_SLICES) * s )}));

            spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.01;
        }
    }

    float getSpinFactor(unsigned short index){ (index < MATHART_SLICES)? spinFactors[index] : 0.0F; }
private:
    Geo_Surface* circleMeshes[MATHART_SLICES];
    float spinFactors[MATHART_SLICES];
};

struct MathArt_Demo : public Topl_Program {
    MathArt_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "MathArt", backend){
        for(unsigned s = 0; s < MATHART_SLICES; s++){
            circleMeshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, CIRCLE_SEGMENTS + ((s * 64) % CIRCLE_SEGMENTS)});
            circleActors[s] = Geo_Actor(circleMeshes[s]);
            circleActors[s].setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / MATHART_SLICES) * s )}));
            // circleActors[s].updateRot({ 0.0F, 0.5F, 0.0F });
            spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.01;
#ifdef RASTERON_H
            /* Rasteron_Image* sliceImg = solidImgOp({ MATHART_SLICES, MATHART_SLICES }, colors_blend(0xFF111111, 0xFF11EE11, (1.0 /  MATHART_SLICES) * s));
            volumeImg.addSlice(sliceImg, s);
            RASTERON_DEALLOC(sliceImg); */
#endif
        }
    }

    void init() override;
    void loop(double frameTime) override;

    // MathArt_Construct topContruct = MathArt_Construct();

    Geo_Surface* circleMeshes[MATHART_SLICES];
    Geo_Actor circleActors[MATHART_SLICES];
    float spinFactors[MATHART_SLICES];

    Img_Volume volumeImg = Img_Volume(MATHART_SLICES);
private:
    Topl_Scene scene;
    // Topl_Scene targetScene;
} *_DEMO;