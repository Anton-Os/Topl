#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define BRUSH3D_SIZE 0.25F
#define BRUSH3D_SCALE 1
#define BRUSH3D_SEGMENTS 30
#define BRUSH3D_SHIFT 0.25F // (rand() / RAND_MAX) * 1.0F
#define BRUSH3D_ROT 0.26F

struct Brush3D_Demo : public Topl_Program {
    Brush3D_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Brush3D", backend){
        for(unsigned b = 0; b < BRUSH3D_SEGMENTS; b++){
            trigBrushes[1]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            trigBrushes[1]->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
            trigBrushes[0]->extend(*trigBrushes[1]);
            quadBrushes[1]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            quadBrushes[1]->rotate({ -BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
            quadBrushes[0]->extend(*quadBrushes[1]);
            hexBrushes[1]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            hexBrushes[1]->rotate({ BRUSH3D_ROT, -BRUSH3D_ROT, BRUSH3D_ROT });
            hexBrushes[0]->extend(*hexBrushes[1]);
            circleBrushes[1]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            circleBrushes[1]->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, -BRUSH3D_ROT });
            circleBrushes[0]->extend(*circleBrushes[1]);
        }

        brushes3D[0] = Geo_Actor(trigBrushes[0]);
        brushes3D[1] = Geo_Actor(quadBrushes[0]);
        brushes3D[2] = Geo_Actor(hexBrushes[0]);
        brushes3D[3] = Geo_Actor(circleBrushes[0]);
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;

    Geo_Trig3D* trigBrushes[2] = { new Geo_Trig3D(BRUSH3D_SIZE), new Geo_Trig3D(BRUSH3D_SIZE / BRUSH3D_SCALE) };
    Geo_Quad3D* quadBrushes[2] = { new Geo_Quad3D(BRUSH3D_SIZE), new Geo_Quad3D(BRUSH3D_SIZE / BRUSH3D_SCALE) };
    Geo_Hex3D* hexBrushes[2] = { new Geo_Hex3D(BRUSH3D_SIZE), new Geo_Hex3D(BRUSH3D_SIZE / BRUSH3D_SCALE) };
    Geo_Circle3D* circleBrushes[2] = { new Geo_Circle3D(BRUSH3D_SIZE), new Geo_Circle3D(BRUSH3D_SIZE / BRUSH3D_SCALE) };

    Geo_Actor brushes3D[4];
private:
    void onAnyKey(char key);

    Topl_Scene scene = PROGRAM_SCENE;
} *_DEMO;