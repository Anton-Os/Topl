#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define TRAVERSAL_RADIUS 5.0F
#define TRAVERSAL_DEPTH 10.0F

struct Traversal_Demo : public Topl_Program {
    Traversal_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Traversal", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    Geo_Quad3D squareCorridor = Geo_Quad3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH);
    Geo_Hex3D hexCorridor = Geo_Hex3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH);
    Geo_Circle3D circleCorridor = Geo_Circle3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH);

    Geo_Actor corridorActors[3] = {
        Geo_Actor(&squareCorridor), Geo_Actor(&hexCorridor), Geo_Actor(&circleCorridor)
    };

    Topl_Scene scene = PROGRAM_SCENE;
} *Traversal;