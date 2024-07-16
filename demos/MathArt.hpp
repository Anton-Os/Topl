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

    void configure(Topl_Scene* scene){
        for(unsigned s = 0; s < MATHART_SLICES; s++) scene->addGeometry("circle" + std::to_string(s), &_geoActors[s]);
    }

    float getSpinFactor(unsigned short index){ return (index < MATHART_SLICES)? spinFactors[index] : 0.0F; }
private:
    Geo_Surface* circleMeshes[MATHART_SLICES];
    float spinFactors[MATHART_SLICES];
};

struct MathArt_Demo : public Topl_Program {
    MathArt_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "MathArt", backend){}

    void init() override;
    void loop(double frameTime) override;

    MathArt_Construct construct = MathArt_Construct();

    Geo_Surface* circleMeshes[MATHART_SLICES];
    Geo_Actor circleActors[MATHART_SLICES];
    float spinFactors[MATHART_SLICES];

    Img_Volume volumeImg = Img_Volume(MATHART_SLICES);
private:
    Topl_Scene scene;
    // Topl_Scene targetScene;
} *_DEMO;