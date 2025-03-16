#include "Geo_Construct.hpp"
#include "meshes/Geo_Surface.hpp"

#include "program/Topl_Program.hpp"

#define KALEIDOSCOPE_Z 0.5F
#define KALEIDOSCOPE_SLICES 256 / 4
#define KALEIDOSCOPE_DIVS 1

struct Kaleidoscope_Construct : public Geo_Construct {
    Kaleidoscope_Construct() : Geo_Construct("Top" + std::to_string(rand() % 999)){ init(); }

    Kaleidoscope_Construct(unsigned short div) : Geo_Construct("Top" + std::to_string(rand() % 999)){
        minDivs = div;
        maxDivs = div;
        init();
    }

    Kaleidoscope_Construct(unsigned short div1, unsigned short div2) : Geo_Construct("Top" + std::to_string(rand() % 999)){
        minDivs = (div1 < div2)? div1 : div2;
        maxDivs = (div1 < div2)? div2 : div1;
        init();
    }

    void init() override {
        for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++){
            float z = KALEIDOSCOPE_Z;

            // meshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, CIRCLE_SEGMENTS + ((s * 64) % CIRCLE_SEGMENTS)});
            // meshes[s] = new Geo_Surface({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs }, KALEIDOSCOPE_Z);
            meshes[s] = new Geo_Ext2D({ (float)rand() / (float)RAND_MAX, (minDivs == maxDivs)? minDivs : (unsigned)(rand() % maxDivs) + minDivs }, z, KALEIDOSCOPE_DIVS);
            _geoActors.push_back(Geo_Actor(meshes[s]));
            _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / KALEIDOSCOPE_SLICES) * s )}));

            spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.01;
        }
    }

    void configure(Topl_Scene* scene){
        for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++) scene->addGeometry(getPrefix() +"circle" + std::to_string(s), &_geoActors[s]);
    }

    float getSpinFactor(unsigned short index){ return (index < KALEIDOSCOPE_SLICES)? spinFactors[index] : 0.0F; }
private:
    unsigned short minDivs = CIRCLE_SEGMENTS;
    unsigned short maxDivs = CIRCLE_SEGMENTS;

    // Geo_Surface* meshes[KALEIDOSCOPE_SLICES];
    Geo_Ext2D* meshes[KALEIDOSCOPE_SLICES];
    float spinFactors[KALEIDOSCOPE_SLICES];
};

struct Kaleidoscope_Demo : public Topl_Program {
    Kaleidoscope_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Kaleidoscope", backend){}

    void init() override;
    void loop(double frameTime) override;

    Kaleidoscope_Construct* getConstruct(){
        // std::cout << "Mode is " << std::to_string(mode) << std::endl;
        switch(mode % 5){
            case 0: return &construct1;
            case 1: return &construct2;
            case 2: return &construct3;
            case 3: return &construct4;
            default: return nullptr; 
        }
    }

    void setConstruct(unsigned m){
        mode = m;
        construct1.toggleShow((m == 0)? true : false);
        construct2.toggleShow((m == 1)? true : false);
        construct3.toggleShow((m == 2)? true : false);
        construct4.toggleShow((m == 3)? true : false);
    }

    Kaleidoscope_Construct construct1 = Kaleidoscope_Construct(6, 32);
    Kaleidoscope_Construct construct2 = Kaleidoscope_Construct(4);
    Kaleidoscope_Construct construct3 = Kaleidoscope_Construct(CIRCLE_SEGMENTS);
    Kaleidoscope_Construct construct4 = Kaleidoscope_Construct(3);

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
