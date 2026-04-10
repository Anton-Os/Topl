#include "Geo_Construct.hpp"
#include "meshes/Geo_Surface.hpp"

#include "program/Topl_Program.hpp"

#define KALEIDOSCOPE_Z 0.5F
#define KALEIDOSCOPE_SLICES 256 / 4
#define KALEIDOSCOPE_TESS 1
#define KALEIDOSCOPE_NAME "kaleidoscope" + std::to_string(rand() % 9999)

#include "Kaleidoscope_Construct.h"

struct Kaleidoscope_Demo : public Topl_Program {
    Kaleidoscope_Demo(const char* execPath) : Topl_Program(execPath, "Kaleidoscope") {}
    Kaleidoscope_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Kaleidoscope", backend){}

    void init() override;
    void loop(double frameTime) override;

    Kaleidoscope_Construct constructs[9] = {
        Kaleidoscope_Construct(3), 
        Kaleidoscope_Construct(4),
        Kaleidoscope_Construct(std::make_pair(6, 32)), 
        Kaleidoscope_Construct(CIRCLE_SEGMENTS),
        Kaleidoscope_Construct(3, KALEIDOSCOPE_TESS), 
        Kaleidoscope_Construct(4, KALEIDOSCOPE_TESS),
        Kaleidoscope_Construct(std::make_pair(6, 32), KALEIDOSCOPE_TESS), 
        Kaleidoscope_Construct(CIRCLE_SEGMENTS, KALEIDOSCOPE_TESS),
        Kaleidoscope_Construct(std::make_pair(3, CIRCLE_SEGMENTS), -KALEIDOSCOPE_TESS)
    };
#ifdef TOPL_ENABLE_TEXTURES
    Sampler_3D volumeImg = Sampler_3D(KALEIDOSCOPE_SLICES);
#endif
private:
    void onAnyKey(keyboard_t key);
    void onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = Topl_Scene(); // PROGRAM_SCENE;
    // Topl_Scene targetScene;

    Flat_VertexShader vertexShader; Flat_PixelShader pixelShader;
    Advance_TessCtrlShader tessCtrlShader;
    Advance_TessEvalShader tessEvalShader;
    Topl_Pipeline* tessPipeline;
} *Kaleidoscope;
