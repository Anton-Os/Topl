#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "program/Topl_Program.hpp"

#define BRUSH3D_SIZE 0.25F
#define BRUSH3D_SCALE 2
#define BRUSH3D_ITERS 50
#define BRUSH3D_SEGMENTS 100
#define BRUSH3D_SHIFT 0.25F
#define BRUSH3D_ROT 0.05F

Vec3f getRandTransform(float amount){
    static unsigned invocation = 0;

    switch (invocation % 6) {
        case 0: return Vec3f({ amount, 0.0F, 0.0F });
        case 1: return Vec3f({ -amount, 0.0F, 0.0F });
        case 2: return Vec3f({ 0.0F, amount, 0.0F });
        case 3: return Vec3f({ 0.0F, -amount, 0.0F });
        case 4: return Vec3f({ 0.0F, 0.0F, amount });
        case 5: return Vec3f({ 0.0F, 0.0F, -amount });
        default: return Vec3f({ -amount, -amount, -amount });
    }

    invocation++;
}

struct Penscape_Demo : public Topl_Program {
    Penscape_Demo(const char* execPath) : Topl_Program(execPath, "Penscape"){}
    Penscape_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Penscape", backend){}

    void init() override;
    void loop(double frameTime) override;

    Geo_Mesh* trigMeshes[3] = { new Geo_Trig3D(BRUSH3D_SIZE), new Geo_TrigCone(BRUSH3D_SIZE), new Geo_TrigOrb(BRUSH3D_SIZE) };
    Geo_Mesh* trigMeshBrushes[3] = {
        new Geo_Trig3D(BRUSH3D_SIZE / BRUSH3D_SCALE, BRUSH3D_SIZE),
        new Geo_TrigCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_TrigOrb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };
    Geo_Mesh* quadMeshes[3] = { new Geo_Quad3D(BRUSH3D_SIZE), new Geo_QuadCone(BRUSH3D_SIZE), new Geo_QuadOrb(BRUSH3D_SIZE) }; 
    Geo_Mesh* quadMeshBrushes[3] = {
        new Geo_Quad3D(BRUSH3D_SIZE / BRUSH3D_SCALE, BRUSH3D_SIZE),
        new Geo_QuadCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_QuadOrb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };
    /* Geo_Mesh* hexMeshes[3] = { new Geo_Hex3D(BRUSH3D_SIZE), new Geo_HexCone(BRUSH3D_SIZE), new Geo_HexOrb(BRUSH3D_SIZE) }; 
    Geo_Mesh* hexMeshBrushes[3] = {
        new Geo_Hex3D(BRUSH3D_SIZE / BRUSH3D_SCALE, BRUSH3D_SIZE),
        new Geo_HexCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_HexOrb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    }; */
    Geo_Mesh* circleMeshes[3] = { new Geo_Circle3D(BRUSH3D_SIZE), new Geo_CircleCone(BRUSH3D_SIZE), new Geo_Orb(BRUSH3D_SIZE) }; 
    Geo_Mesh* circleMeshBrushes[3] = {
        new Geo_Circle3D(BRUSH3D_SIZE / BRUSH3D_SCALE, BRUSH3D_SIZE),
        new Geo_CircleCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_Orb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };

    Geo_Actor brushes3D[3][3];
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;
    void createBrushes();

    Topl_Scene scene = PROGRAM_SCENE;
} *Penscape;