#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define MATHART_SLICES 256

struct MathArt_Demo : public Topl_Program {
    MathArt_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "MathArt", backend){
        for(unsigned s = 0; s < MATHART_SLICES; s++){
            circleMeshes[s] = new Geo_Circle2D(rand() / RAND_MAX);
            circleActors[s] = new Geo_Actor(&circleMeshes[s]);
            circleActors[s].setPos(Vec3f(0.0F, 0.0F, -1.0 + ((2.0 / MATHART_SLICES) * s)));
#ifdef RASTERON_H
            Rasteron_Image* sliceImg = solidImgOp({ MATHART_SLICES, MATHART_SLICES }, colors_blend(0xFF111111, 0xFF11EE11, (1.0 / MATHART_SLICES) * s));
            volumeImg.addSlice(sliceImg, s);
            RASTERON_DEALLOC(s);
#endif
        }
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Circle2D* circleMeshes[MATHART_SLICES];
    Geo_Actor* circleActors[MATHART_SLICES];

    Img_Volume volumeImg = Img_Volume(MATHART_SLICES);
private:
    Topl_Scene scene;
    // Topl_Scene targetScene;
} *_DEMO;