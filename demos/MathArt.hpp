#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define MATHART_SLICES 256

struct MathArt_Demo : public Topl_Program {
    MathArt_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "MathArt", backend){}

    void init() override;
    void loop(double frameTime) override;

    Geo_Circle2D circle = Geo_Circle2D(1.25);
    Geo_Actor circleActor = Geo_Actor(&circle);
    Geo_Trig2D triangle = Geo_Trig2D(1.25);
    Geo_Actor triangleActor = Geo_Actor(&triangle);
    Geo_Quad2D box = Geo_Quad2D(1.25);
    Geo_Actor boxActor = Geo_Actor(&box);

    Geo_Mesh* surfaceMesh = nullptr;
    Geo_Mesh* coneMesh = nullptr;
    Geo_Mesh* volumeMesh = nullptr;
    Geo_Actor targetActor;

    Img_Volume volumeImg = Img_Volume(MATHART_SLICES);
private:
    Topl_Scene scene;
    Topl_Scene targetScene;
} *_DEMO;