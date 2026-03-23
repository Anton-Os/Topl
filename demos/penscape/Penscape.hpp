#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"

#include "program/Topl_Program.hpp"

#define PENSCAPE_SIZE 0.25F
#define PENSCAPE_SCALE 2
#define PENSCAPE_ITERS 50
#define PENSCAPE_SEGMENTS 100
#define PENSCAPE_SHIFT 0.25F
#define PENSCAPE_ROT 0.03F

// #define PENSCAPE_MESH_INSTANCES Mat4x4::translation(Vec3f({ -0.25F, 0.0F, 0.0F})), Mat4x4::translation(Vec3f({ 0.25F, 0.0F, 0.0F})), Mat4x4::translation(Vec3f({ 0.0F, 0.25F, 0.0F})), Mat4x4::translation(Vec3f({ 0.0F, 0.25F, 0.0F}))
#define PENSCAPE_MESH_INSTANCES Mat4x4::scale(Vec3f({ 1.25F, 1.0F, 1.0F})), Mat4x4::translation(Vec3f({ 1.0F, 1.25F, 1.0F })), Mat4x4::translation(Vec3f({ 1.0F, 1.0F, 1.25F }))

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

    Geo_Mesh* trigMeshes[3] = { new Geo_Trig3D(PENSCAPE_SIZE), new Geo_TrigCone(PENSCAPE_SIZE), new Geo_TrigOrb(PENSCAPE_SIZE) };  
    Geo_Mesh* trigMeshBrushes[3] = {
        new Geo_Trig3D(PENSCAPE_SIZE / PENSCAPE_SCALE, PENSCAPE_SIZE),
        new Geo_TrigCone(PENSCAPE_SIZE / PENSCAPE_SCALE, Vec3f({ 0.0F, 0.0F, PENSCAPE_SIZE })),
        new Geo_TrigOrb(PENSCAPE_SIZE / PENSCAPE_SCALE)
    };
    Geo_Mesh* quadMeshes[3] = { new Geo_Quad3D(PENSCAPE_SIZE), new Geo_QuadCone(PENSCAPE_SIZE), new Geo_QuadOrb(PENSCAPE_SIZE) }; 
    Geo_Mesh* quadMeshBrushes[3] = {
        new Geo_Quad3D(PENSCAPE_SIZE / PENSCAPE_SCALE, PENSCAPE_SIZE),
        new Geo_QuadCone(PENSCAPE_SIZE / PENSCAPE_SCALE, Vec3f({ 0.0F, 0.0F, PENSCAPE_SIZE })),
        new Geo_QuadOrb(PENSCAPE_SIZE / PENSCAPE_SCALE)
    };
    /* Geo_Mesh* hexMeshes[3] = { new Geo_Hex3D(PENSCAPE_SIZE), new Geo_HexCone(PENSCAPE_SIZE), new Geo_HexOrb(PENSCAPE_SIZE) }; 
    Geo_Mesh* hexMeshBrushes[3] = {
        new Geo_Hex3D(PENSCAPE_SIZE / PENSCAPE_SCALE, PENSCAPE_SIZE),
        new Geo_HexCone(PENSCAPE_SIZE / PENSCAPE_SCALE, Vec3f({ 0.0F, 0.0F, PENSCAPE_SIZE })),
        new Geo_HexOrb(PENSCAPE_SIZE / PENSCAPE_SCALE)
    }; */
    Geo_Mesh* circleMeshes[3] = { new Geo_Circle3D(PENSCAPE_SIZE), new Geo_CircleCone(PENSCAPE_SIZE), new Geo_Orb(PENSCAPE_SIZE) }; 
    Geo_Mesh* circleMeshBrushes[3] = {
        new Geo_Circle3D(PENSCAPE_SIZE / PENSCAPE_SCALE, PENSCAPE_SIZE),
        new Geo_CircleCone(PENSCAPE_SIZE / PENSCAPE_SCALE, Vec3f({ 0.0F, 0.0F, PENSCAPE_SIZE })),
        new Geo_Orb(PENSCAPE_SIZE / PENSCAPE_SCALE)
    };

    Geo_Actor brushes3D[3][3];
private:
    void onAnyKey(keyboard_t key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;
    void createBrushes();

    Topl_Scene scene = PROGRAM_SCENE;
} *Penscape;