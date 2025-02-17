#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define BRUSH3D_SIZE 0.25F
#define BRUSH3D_SCALE 1
#define BRUSH3D_SEGMENTS 30
#define BRUSH3D_SHIFT 0.25F // (rand() / RAND_MAX) * 1.0F
#define BRUSH3D_ROT 0.26F

Geo_Vertex vertexTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation){
    return Geo_Vertex(vertex.position * (1.0F / invocation)); // test
}

Geo_Vertex midpointTform(const Geo_Vertex& vertex, const Geo_Vertex& midpoint, unsigned primID, unsigned invocation){
    return Geo_Vertex(vertex.position * (1.0F / primID)); // test
}

struct Penscape_Demo : public Topl_Program {
    Penscape_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Penscape", backend){
        for(unsigned b = 0; b < BRUSH3D_SEGMENTS; b++){
            trigMeshBrush->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            trigMeshBrush->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
            for(unsigned a = 0; a < 3; a++) trigMeshes[a]->extend(*trigMeshBrush);
            quadMeshBrush->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            quadMeshBrush->rotate({ -BRUSH3D_ROT, BRUSH3D_ROT, BRUSH3D_ROT });
            for(unsigned a = 0; a < 3; a++) quadMeshes[a]->extend(*quadMeshBrush);
            hexMeshBrush->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            hexMeshBrush->rotate({ BRUSH3D_ROT, -BRUSH3D_ROT, BRUSH3D_ROT });
            for(unsigned a = 0; a < 3; a++) hexMeshes[a]->extend(*hexMeshBrush);
            circleMeshBrush->shift({ 0.0F, 0.0F, BRUSH3D_SHIFT});
            circleMeshBrush->rotate({ BRUSH3D_ROT, BRUSH3D_ROT, -BRUSH3D_ROT });
            for(unsigned a = 0; a < 3; a++) circleMeshes[a]->extend(*circleMeshBrush);
        }

        for(unsigned a = 0; a < 3; a++){
            if(a > 0) (a % 2 == 1)? trigMeshes[a]->modify(vertexTform) : trigMeshes[a]->modify(midpointTform);
            brushes3D[a][0] = Geo_Actor(trigMeshes[a]);
            if(a > 0) (a % 2 == 1)? quadMeshes[a]->modify(vertexTform) : quadMeshes[a]->modify(midpointTform);
            brushes3D[a][1] = Geo_Actor(quadMeshes[a]);
            if(a > 0) (a % 2 == 1)? hexMeshes[a]->modify(vertexTform) : hexMeshes[a]->modify(midpointTform);
            brushes3D[a][2] = Geo_Actor(hexMeshes[a]);
            if(a > 0) (a % 2 == 1)? circleMeshes[a]->modify(vertexTform) : circleMeshes[a]->modify(midpointTform);
            brushes3D[a][3] = Geo_Actor(circleMeshes[a]);
        }
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;

    Geo_Trig3D* trigMeshes[3] = { new Geo_Trig3D(BRUSH3D_SIZE), new Geo_Trig3D(BRUSH3D_SIZE), new Geo_Trig3D(BRUSH3D_SIZE) }; 
    Geo_Trig3D* trigMeshBrush = new Geo_Trig3D(BRUSH3D_SIZE / BRUSH3D_SCALE);
    Geo_Quad3D* quadMeshes[3] = { new Geo_Quad3D(BRUSH3D_SIZE), new Geo_Quad3D(BRUSH3D_SIZE), new Geo_Quad3D(BRUSH3D_SIZE) }; 
    Geo_Quad3D* quadMeshBrush = new Geo_Quad3D(BRUSH3D_SIZE / BRUSH3D_SCALE);
    Geo_Hex3D* hexMeshes[3] = { new Geo_Hex3D(BRUSH3D_SIZE), new Geo_Hex3D(BRUSH3D_SIZE), new Geo_Hex3D(BRUSH3D_SIZE) }; 
    Geo_Hex3D* hexMeshBrush = new Geo_Hex3D(BRUSH3D_SIZE / BRUSH3D_SCALE);
    Geo_Circle3D* circleMeshes[3] = { new Geo_Circle3D(BRUSH3D_SIZE), new Geo_Circle3D(BRUSH3D_SIZE), new Geo_Circle3D(BRUSH3D_SIZE) }; 
    Geo_Circle3D* circleMeshBrush = new Geo_Circle3D(BRUSH3D_SIZE / BRUSH3D_SCALE);

    Geo_Actor brushes3D[3][4];
private:
    void onAnyKey(char key);

    Topl_Scene scene = PROGRAM_SCENE;
} *_DEMO;