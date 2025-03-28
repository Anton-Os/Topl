#include "Geo_Construct.hpp"
#include "meshes/Geo_Surface.hpp"

#include "program/Topl_Program.hpp"

#define KALEIDOSCOPE_Z 0.5F
#define KALEIDOSCOPE_SLICES 256 / 4
#define KALEIDOSCOPE_TESS 1

struct Kaleidoscope_Construct : public Geo_Construct {
    Kaleidoscope_Construct(unsigned short div) : Geo_Construct("Top" + std::to_string(rand() % 999)), divisions(std::make_pair(div, div)){ init(); }

    Kaleidoscope_Construct(std::pair<unsigned short, unsigned short> divs) : Geo_Construct("Top" + std::to_string(rand() % 999)), divisions(divs){ init(); }

    Kaleidoscope_Construct(unsigned short div, unsigned short t) : Geo_Construct("Top" + std::to_string(rand() % 999)), divisions(std::make_pair(div, div)){
        tessCount = t;
        init();
    }

    Kaleidoscope_Construct(std::pair<unsigned short, unsigned short> divs, unsigned short t) : Geo_Construct("Top" + std::to_string(rand() % 999)), divisions(divs){
        tessCount = t;
        init();
    }

    void init() override {
        unsigned short minDivs = (divisions.first < divisions.second)? divisions.first : divisions.second;
        unsigned short maxDivs = (divisions.first < divisions.second)? divisions.second : divisions.first;

        for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++){
            float z = KALEIDOSCOPE_Z;

            if(tessCount != 0) meshes[s] = new Geo_Ext2D({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs }, z, tessCount);
            else meshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs }, z);
            // else // create a custom mesh
            _geoActors.push_back(Geo_Actor(meshes[s]));
            _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / KALEIDOSCOPE_SLICES) * s )}));

            spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.01;
        }
    }

    void configure(Topl_Scene* scene){
        for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++) scene->addGeometry(getPrefix() + "slice" + std::to_string(s), &_geoActors[s]);
    }

    float getSpinFactor(unsigned short index){ return (index < KALEIDOSCOPE_SLICES)? spinFactors[index] : 0.0F; }
private:
    const std::pair<unsigned short, unsigned short> divisions;
    unsigned short tessCount = 0;

    // Geo_Surface* meshes[KALEIDOSCOPE_SLICES];
    Geo_Mesh* meshes[KALEIDOSCOPE_SLICES];
    float spinFactors[KALEIDOSCOPE_SLICES];
};

struct Kaleidoscope_Demo : public Topl_Program {
    Kaleidoscope_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Kaleidoscope", backend){}

    void init() override;
    void loop(double frameTime) override;

    void setConstruct(unsigned m){
        mode = m;
        for(unsigned c = 0; c < 9; c++) constructs[c].toggleShow(m == c);
    }

    Kaleidoscope_Construct constructs[9] = {
        Kaleidoscope_Construct(3), 
        Kaleidoscope_Construct(4),
        Kaleidoscope_Construct(std::make_pair(6, 32)), 
        Kaleidoscope_Construct(CIRCLE_SEGMENTS),
        Kaleidoscope_Construct(3, KALEIDOSCOPE_TESS), 
        Kaleidoscope_Construct(4, KALEIDOSCOPE_TESS),
        Kaleidoscope_Construct(std::make_pair(6, 32), KALEIDOSCOPE_TESS), 
        Kaleidoscope_Construct(CIRCLE_SEGMENTS, KALEIDOSCOPE_TESS),
        Kaleidoscope_Construct(std::make_pair(3, CIRCLE_SEGMENTS), KALEIDOSCOPE_TESS)
    };

    Sampler_3D volumeImg = Sampler_3D(KALEIDOSCOPE_SLICES);
private:
    void onAnyKey(char key);
    void onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = Topl_Scene(); // PROGRAM_SCENE;
    // Topl_Scene targetScene;

    Flat_VertexShader vertexShader; Flat_PixelShader pixelShader;
    Advance_TessCtrlShader tessCtrlShader;
    Advance_TessEvalShader tessEvalShader;
    Topl_Pipeline* tessPipeline;
} *_DEMO;
