#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "Topl_Demo.hpp"

#define CUBEGATE_SIZE 1.0F
#define CUBEGATE_ORBSIZE 0.1F
#define CUBEGATE_FLOORSIZE 10.0F
#define CUBEGATE_BALLS 8
#define CUBEGATE_SCENES 6
#define CUBEGATE_TESS 4
#define CUBEGATE_COUNT 9
#define CUBEGATE_NAME "cubegate" + std::to_string(rand() % 9999)

#include "Cubegate_Construct.h"

struct Cubegate_Demo : public Topl_Demo {
    Cubegate_Demo(const char* execPath) : Topl_Demo(execPath, "Cubegate"){ }
    Cubegate_Demo(const char* execPath, BACKEND_Target backend) : Topl_Demo(execPath, "Cubegate", backend) {}

    void init() override;
    void loop(double frameTime) override;

    void preloop() override;

    Geo_Quad2D faceMeshes[CUBEGATE_COUNT] = {
        Geo_Quad2D(CUBEGATE_SIZE), Geo_Quad2D(CUBEGATE_SIZE * 0.9), Geo_Quad2D(CUBEGATE_SIZE * 0.8),
        Geo_Quad2D(CUBEGATE_SIZE * 0.7), Geo_Quad2D(CUBEGATE_SIZE * 0.6), Geo_Quad2D(CUBEGATE_SIZE * 0.5),
        Geo_Quad2D(CUBEGATE_SIZE * 0.4), Geo_Quad2D(CUBEGATE_SIZE * 0.3), Geo_Quad2D(CUBEGATE_SIZE * 0.2)
    };
    // Geo_Hex2D faceMesh2 = Geo_Hex2D(CUBEGATE_SIZE);
    // Geo_Circle2D faceMesh3 = Geo_Circle2D(CUBEGATE_SIZE);
    Geo_Quad3D cubeMesh = Geo_Quad3D(CUBEGATE_SIZE * 0.66F, CUBEGATE_SIZE * 0.66F);

    // Geo_Actor faceActors[6] = {&faceMesh, &faceMesh, &faceMesh, &faceMesh, &faceMesh, &faceMesh};
    Geo_Actor cubeActor = Geo_Actor(&cubeMesh);

    Geo_Orb orbMesh = Geo_Orb(CUBEGATE_ORBSIZE);
    Geo_Circle2D floorMesh = Geo_Circle2D(CUBEGATE_FLOORSIZE);
    Geo_Circle3D wallMesh = Geo_Circle3D(CUBEGATE_FLOORSIZE, CUBEGATE_FLOORSIZE / 2.0F);

    Geo_Actor balls[CUBEGATE_BALLS] = {
        Geo_Actor(&orbMesh), Geo_Actor(&orbMesh), Geo_Actor(&orbMesh), Geo_Actor(&orbMesh),
        Geo_Actor(&orbMesh), Geo_Actor(&orbMesh), Geo_Actor(&orbMesh), Geo_Actor(&orbMesh)
    };
    Geo_Actor floor = Geo_Actor(&floorMesh);
    Geo_Actor ceiling = Geo_Actor(&floorMesh);
    Geo_Actor walls = Geo_Actor(&wallMesh);

    Cubegate_Construct cubeGates[CUBEGATE_COUNT] = {
        Cubegate_Construct(&faceMeshes[0]), Cubegate_Construct(&faceMeshes[1]), Cubegate_Construct(&faceMeshes[2]),
        Cubegate_Construct(&faceMeshes[3]), Cubegate_Construct(&faceMeshes[4]), Cubegate_Construct(&faceMeshes[5]),
        Cubegate_Construct(&faceMeshes[6]), Cubegate_Construct(&faceMeshes[7]), Cubegate_Construct(&faceMeshes[8]),
    };

    // Cubegate_Construct cubeGatesArray[CUBEGATE_COUNT][CUBEGATE_COUNT] = // TODO: Switch to this for more complex surfaces
#ifdef TOPL_ENABLE_TEXTURES
    Topl_Sampler_3D cubeTex = Topl_Sampler_3D(256);

    Topl_Sampler_2D frontFaceTex = Topl_Sampler_Gradient(SIDE_Radial, (RAND_COLOR() & 0xFFFFFF) | 0xDD000000, (RAND_COLOR() & 0xFFFFFF) | 0x22000000);
    Topl_Sampler_2D backFaceTex = Topl_Sampler_Gradient(SIDE_Radial, (RAND_COLOR() & 0xFFFFFF) | 0xDD000000, (RAND_COLOR() & 0xFFFFFF) | 0x22000000);
    Topl_Sampler_2D topFaceTex = Topl_Sampler_Gradient(SIDE_Radial, (RAND_COLOR() & 0xFFFFFF) | 0xDD000000, (RAND_COLOR() & 0xFFFFFF) | 0x22000000);
    Topl_Sampler_2D bottomFaceTex = Topl_Sampler_Gradient(SIDE_Radial, (RAND_COLOR() & 0xFFFFFF) | 0xDD000000, (RAND_COLOR() & 0xFFFFFF) | 0x22000000);
    Topl_Sampler_2D leftFaceTex = Topl_Sampler_Gradient(SIDE_Radial, (RAND_COLOR() & 0xFFFFFF) | 0xDD000000, (RAND_COLOR() & 0xFFFFFF) | 0x22000000);
    Topl_Sampler_2D rightFaceTex = Topl_Sampler_Gradient(SIDE_Radial, (RAND_COLOR() & 0xFFFFFF) | 0xDD000000, (RAND_COLOR() & 0xFFFFFF) | 0x22000000);
#endif
private:
    void onAnyKey(keyboard_t key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = PROGRAM_SCENE;

    Topl_Camera worldCamera = Topl_Camera(PROJECTION_Perspective);
    Topl_Scene worldScenes[CUBEGATE_SCENES] = {
        Topl_Scene(&worldCamera), Topl_Scene(&worldCamera), Topl_Scene(&worldCamera),
        Topl_Scene(&worldCamera), Topl_Scene(&worldCamera), Topl_Scene(&worldCamera)
    };
} *Cubegate;
