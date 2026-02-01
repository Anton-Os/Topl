#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "program/Topl_Program.hpp"

#define TRAVERSAL_RADIUS 5.0F
#define TRAVERSAL_DEPTH 100.0F * 2
#define TRAVERSAL_OBSCACLES 20
#define TRAVERSAL_TESS 3
#define TRAVERSAL_SPEED 0.0000001F
#define TRAVERSAL_CORRIDORS 3
#define TRAVERSAL_RECURSION 8

struct Traversal_Demo : public Topl_Program {
    Traversal_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Traversal", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Geo_Quad3D squareCorridor = Geo_Quad3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH);
    Geo_Hex3D hexCorridor = Geo_Hex3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH);
    Geo_Circle3D circleCorridor = Geo_Circle3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH);

    Geo_Actor actors[3] = { Geo_Actor(&squareCorridor), Geo_Actor(&hexCorridor), Geo_Actor(&circleCorridor) };

    Geo_Actor corridorActors[TRAVERSAL_CORRIDORS][TRAVERSAL_RECURSION] = {
        { actors[0], actors[0], actors[0], actors[0], actors[0], actors[0], actors[0], actors[0] },
        { actors[1], actors[1], actors[1], actors[1], actors[1], actors[1], actors[1], actors[1] },
        { actors[2], actors[2], actors[2], actors[2], actors[2], actors[2], actors[2], actors[2] },
    };

    // Geo_Orb obstacle;
    // Geo_Actor obstacleActors[TRAVERSAL_OBSTACLES];

    Topl_Scene scene = PROGRAM_SCENE;
} *Traversal;