#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define TRAVERSAL_RADIUS 5.0F
#define TRAVERSAL_DEPTH 100.0F * 2
#define TRAVERSAL_OBSCACLES 20
#define TRAVERSAL_TESS 3
#define TRAVERSAL_SPEED 0.0000001F
#define TRAVERSAL_SLICES 100
#define TRAVERSAL_CORRIDORS 3
#define TRAVERSAL_RECURSION 9
#define TRAVERSAL_ALPHA 0xAA000000

struct Traversal_Demo : public Topl_Program {
    Traversal_Demo(const char* execPath) : Topl_Program(execPath, "Traversal"){}
    Traversal_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Traversal", backend){}

    ~Traversal_Demo() {
        for (unsigned c = 0; c < TRAVERSAL_CORRIDORS; c++) for (unsigned s = 0; s < TRAVERSAL_SLICES; s++) delete sliceActorPtrs[c][s];
    }

    void init() override;
    void loop(double frameTime) override;
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Geo_Quad2D squareSlice = Geo_Quad2D(TRAVERSAL_RADIUS);
    Geo_Hex2D hexSlice = Geo_Hex2D(TRAVERSAL_RADIUS);
    Geo_Circle2D circleSlice = Geo_Circle2D(TRAVERSAL_RADIUS);

    Geo_Quad3D squareCorridors[TRAVERSAL_CORRIDORS] = {
        Geo_Quad3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH), Geo_Quad3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH), Geo_Quad3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH)
    };
    Geo_Hex3D hexCorridors[TRAVERSAL_CORRIDORS] = {
        Geo_Hex3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH), Geo_Hex3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH), Geo_Hex3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH)
    };
    Geo_Circle3D circleCorridors[TRAVERSAL_CORRIDORS] = {
        Geo_Circle3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH), Geo_Circle3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH), Geo_Circle3D(TRAVERSAL_RADIUS, TRAVERSAL_DEPTH)
    };

    Geo_Actor actors[TRAVERSAL_RECURSION] = {
        Geo_Actor(&squareCorridors[0]), Geo_Actor(&squareCorridors[1]), Geo_Actor(&squareCorridors[2]),
        Geo_Actor(&hexCorridors[0]), Geo_Actor(&hexCorridors[1]), Geo_Actor(&hexCorridors[2]),
        Geo_Actor(&circleCorridors[0]), Geo_Actor(&circleCorridors[1]), Geo_Actor(&circleCorridors[2]),
    };

    Geo_Actor corridorActors[TRAVERSAL_CORRIDORS][TRAVERSAL_RECURSION] = {
        { actors[0], actors[1], actors[2], actors[0], actors[1], actors[2], actors[0], actors[1], actors[2] },
        { actors[3], actors[4], actors[5], actors[3], actors[4], actors[5], actors[3], actors[4], actors[5] },
        { actors[6], actors[7], actors[8], actors[6], actors[7], actors[8], actors[6], actors[7], actors[8] },
    };
    
    Geo_Actor* sliceActorPtrs[TRAVERSAL_CORRIDORS][TRAVERSAL_SLICES];
#ifdef TOPL_ENABLE_TEXTURES
    Sampler_2D sliceTextures[TRAVERSAL_CORRIDORS] = {
        Sampler_Color(0x11111111 /* (RAND_COLOR() & 0xFFFFFF) | TRAVERSAL_ALPHA */),
        Sampler_Color(0x11111111 /* (RAND_COLOR() & 0xFFFFFF) | TRAVERSAL_ALPHA */),
        Sampler_Color(0x11111111 /* (RAND_COLOR() & 0xFFFFFF) | TRAVERSAL_ALPHA */)
    };
#endif
    Topl_Scene scene = PROGRAM_SCENE;
} *Traversal;