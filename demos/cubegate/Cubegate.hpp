#include "Geo_Construct.hpp"
#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "program/Topl_Program.hpp"

#define CUBEGATE_SIZE 1.0F
#define CUBEGATE_ORBSIZE 0.1F
#define CUBEGATE_FLOORSIZE 10.0F
#define CUBEGATE_BALLS 8
#define CUBEGATE_SCENES 6

enum CUBEGATE_Face {
    CUBEGATE_Front = 0, CUBEGATE_Back = 1, CUBEGATE_Top = 2,
    CUBEGATE_Bottom = 3, CUBEGATE_Left = 4, CUBEGATE_Right = 5,
};

struct Cubegate_Demo : public Topl_Program {
    Cubegate_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Cubegate", backend) {}

    void init() override;
    void loop(double frameTime) override;

    Geo_Quad2D faceMesh = Geo_Quad2D(CUBEGATE_SIZE);
    Geo_Quad3D cubeMesh = Geo_Quad3D(CUBEGATE_SIZE * 0.66F, CUBEGATE_SIZE * 0.66F);

    Geo_Actor faceActors[6] = { &faceMesh, &faceMesh, &faceMesh, &faceMesh, &faceMesh, &faceMesh };
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
#ifdef RASTERON_H
    Sampler_3D cubeTex = Sampler_3D(256);

    Sampler_2D frontFaceTex = Sampler_Color((RAND_COLOR() & 0xFFFFFF) | 0xAA000000);
    Sampler_2D backFaceTex = Sampler_Color((RAND_COLOR() & 0xFFFFFF) | 0xAA000000);
    Sampler_2D topFaceTex = Sampler_Color((RAND_COLOR() & 0xFFFFFF) | 0xAA000000);
    Sampler_2D bottomFaceTex = Sampler_Color((RAND_COLOR() & 0xFFFFFF) | 0xAA000000);
    Sampler_2D leftFaceTex = Sampler_Color((RAND_COLOR() & 0xFFFFFF) | 0xAA000000);
    Sampler_2D rightFaceTex = Sampler_Color((RAND_COLOR() & 0xFFFFFF) | 0xAA000000);
#endif
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = PROGRAM_SCENE;

    Topl_Camera worldCamera;
    Topl_Scene worldScenes[CUBEGATE_SCENES] = {
        Topl_Scene(&worldCamera), Topl_Scene(&worldCamera), Topl_Scene(&worldCamera),
        Topl_Scene(&worldCamera), Topl_Scene(&worldCamera), Topl_Scene(&worldCamera)
    };
} *Cubegate;
