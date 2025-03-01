#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orbitals.hpp"

#include "program/Topl_Program.hpp"

#define BRUSH3D_SIZE 0.25F
#define BRUSH3D_SCALE 3
#define BRUSH3D_SEGMENTS 30
#define BRUSH3D_SHIFT 0.25F // (rand() / RAND_MAX) * 1.0F
#define BRUSH3D_ROT 0.15F

Geo_Vertex vertexTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation){
    // Vec3f randVec = Vec3f({ (((float)rand() / (float)RAND_MAX) - 1.0F) * BRUSH3D_SIZE, (((float)rand() / (float)RAND_MAX) - 1.0F) * BRUSH3D_SIZE, (((float)rand() / (float)RAND_MAX) - 1.0F) * BRUSH3D_SIZE });
    // return Geo_Vertex(vertex.position + randVec); // test
    return (primID == 0)? vertex : Geo_Vertex(vertex.position * (primID % 3));
}

Geo_Vertex midpointTform(const Geo_Vertex& vertex, const Geo_Vertex& midpoint, unsigned primID, unsigned invocation){
    Vec3f randVec = Vec3f({ (((float)rand() / (float)RAND_MAX) - 1.0F) * BRUSH3D_SIZE, (((float)rand() / (float)RAND_MAX) - 1.0F) * BRUSH3D_SIZE, (((float)rand() / (float)RAND_MAX) - 1.0F) * BRUSH3D_SIZE });
    return Geo_Vertex(midpoint.position - randVec); // test
    // return (primID != 0)? vertex : Geo_Vertex(midpoint.position);
}

struct Penscape_Demo : public Topl_Program {
    Penscape_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Penscape", backend){
        trigMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });
        quadMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });
        hexMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });
        circleMeshBrushes[2]->scale({ 1.0F, 1.0F, BRUSH3D_SCALE });

        for(unsigned b = 0; b < BRUSH3D_SEGMENTS; b++)
            for(unsigned a = 0; a < 3 * 2; a++){
                trigMeshBrushes[a % 3]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
                trigMeshBrushes[a % 3]->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
                trigMeshes[a % 3]->extend(*trigMeshBrushes[a % 3]);
                quadMeshBrushes[a % 3]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
                quadMeshBrushes[a % 3]->rotate({ -BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
                quadMeshes[a % 3]->extend(*quadMeshBrushes[a % 3]);
                hexMeshBrushes[a % 3]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
                hexMeshBrushes[a % 3]->rotate({ BRUSH3D_ROT, -BRUSH3D_ROT, BRUSH3D_ROT });
                hexMeshes[a % 3]->extend(*hexMeshBrushes[a % 3]);
                circleMeshBrushes[a % 3]->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
                circleMeshBrushes[a % 3]->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, -BRUSH3D_ROT });
                circleMeshes[a % 3]->extend(*circleMeshBrushes[a % 3]);
            }
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;

    Geo_Mesh* trigMeshes[3] = { new Geo_Trig3D(BRUSH3D_SIZE), new Geo_TrigCone(BRUSH3D_SIZE), new Geo_TrigOrb(BRUSH3D_SIZE) };
    Geo_Mesh* trigMeshBrushes[3] = {
        new Geo_Trig3D(BRUSH3D_SIZE / BRUSH3D_SCALE),
        new Geo_TrigCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_TrigOrb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };
    Geo_Mesh* quadMeshes[3] = { new Geo_Quad3D(BRUSH3D_SIZE), new Geo_QuadCone(BRUSH3D_SIZE), new Geo_QuadOrb(BRUSH3D_SIZE) }; 
    Geo_Mesh* quadMeshBrushes[3] = {
        new Geo_Quad3D(BRUSH3D_SIZE / BRUSH3D_SCALE),
        new Geo_QuadCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_QuadOrb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };
    Geo_Mesh* hexMeshes[3] = { new Geo_Hex3D(BRUSH3D_SIZE), new Geo_HexCone(BRUSH3D_SIZE), new Geo_HexOrb(BRUSH3D_SIZE) }; 
    Geo_Mesh* hexMeshBrushes[3] = {
        new Geo_Hex3D(BRUSH3D_SIZE / BRUSH3D_SCALE),
        new Geo_HexCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_HexOrb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };
    Geo_Mesh* circleMeshes[3] = { new Geo_Circle3D(BRUSH3D_SIZE), new Geo_CircleCone(BRUSH3D_SIZE), new Geo_Orb(BRUSH3D_SIZE) }; 
    Geo_Mesh* circleMeshBrushes[3] = {
        new Geo_Circle3D(BRUSH3D_SIZE / BRUSH3D_SCALE),
        new Geo_CircleCone(BRUSH3D_SIZE / BRUSH3D_SCALE, Vec3f({ 0.0F, 0.0F, BRUSH3D_SIZE })),
        new Geo_Orb(BRUSH3D_SIZE / BRUSH3D_SCALE)
    };

    Geo_Actor brushes3D[3][4];
private:
    void onAnyKey(char key);

    Topl_Scene scene = PROGRAM_SCENE;
} *_DEMO;