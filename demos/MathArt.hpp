#include "Geo_Construct.hpp"
#include "meshes/Geo_Surface.hpp"

#include "program/Topl_Program.hpp"

#define MATHART_SLICES 256 

struct MathArt_Construct : public Geo_Construct {
    MathArt_Construct() : Geo_Construct("Top"){ init(); }

    MathArt_Construct(unsigned short div) : Geo_Construct("Top"){
        minDivs = div;
        maxDivs = div;
        init();
    }

    MathArt_Construct(unsigned short div1, unsigned short div2) : Geo_Construct("Top"){
        minDivs = (div1 < div2)? div1 : div2;
        maxDivs = (div1 < div2)? div2 : div1;
        init();
    }

    void init(){
        for(unsigned s = 0; s < MATHART_SLICES; s++){
            // meshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, CIRCLE_SEGMENTS + ((s * 64) % CIRCLE_SEGMENTS)});
            meshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs });
            _geoActors.push_back(Geo_Actor(meshes[s]));
            _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / MATHART_SLICES) * s )}));

            spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.01;
        }
    }

    void configure(Topl_Scene* scene){
        for(unsigned s = 0; s < MATHART_SLICES; s++) scene->addGeometry("circle" + std::to_string(s), &_geoActors[s]);
    }

    float getSpinFactor(unsigned short index){ return (index < MATHART_SLICES)? spinFactors[index] : 0.0F; }
private:
    unsigned short minDivs = CIRCLE_SEGMENTS;
    unsigned short maxDivs = CIRCLE_SEGMENTS;

    Geo_Surface* meshes[MATHART_SLICES];
    float spinFactors[MATHART_SLICES];
};

struct MathArt_Demo : public Topl_Program {
    MathArt_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "MathArt", backend){
        // construct1.scale({ 1.25F, 1.25F, 1.25F });
        construct2.shift({ 0.75F, 0.0F, 0.0F });
        construct3.shift({ -0.75F, 0.0F, 0.0F });
    }

    void init() override;
    void loop(double frameTime) override;

    MathArt_Construct construct1 = MathArt_Construct(6, 32);
    MathArt_Construct construct2 = MathArt_Construct(4);
    MathArt_Construct construct3 = MathArt_Construct(CIRCLE_SEGMENTS);

    Img_Volume volumeImg = Img_Volume(MATHART_SLICES);
private:
    Topl_Scene scene;
    // Topl_Scene targetScene;
} *_DEMO;