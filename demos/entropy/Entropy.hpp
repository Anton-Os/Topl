#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

#define ENTROPIC_TRIG 0
#define ENTROPIC_QUAD 1
#define ENTROPIC_HEX 2
#define ENTROPIC_CIRCLE 3

#define ENTROPIC_ITERS 1
#define ENTROPIC_SPAWN 100
#define ENTROPIC_COUNT 200
#define ENTROPIC_SCENES 9
#define ENTROPIC_PROB 0.05
#define ENTROPIC_SIZE 0.045
#define ENTROPIC_FORCE 50000.0F
#define ENTROPIC_RECURSHAPE 0.1F
#define ENTROPIC_POINTS 6
#define ENTROPIC_SPREAD 0.5F

struct Entropy_Demo : public Topl_Program {
    Entropy_Demo(const char* execPath) : Topl_Program(execPath, "Entropy"){}
    Entropy_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Entropy", backend) {}

    void init() override;
    void loop(double frameTime) override;

    Topl_Scene* getScene(){ return &scenes[mode % ENTROPIC_SCENES]; }

    Geo_Trig2D trigs[ENTROPIC_COUNT / 4]; Geo_TrigCone trigsCones[ENTROPIC_COUNT / 4]; Geo_Trig3D trigs3D[ENTROPIC_COUNT / 4];
    Geo_Quad2D quads[ENTROPIC_COUNT / 4]; Geo_QuadCone quadsCones[ENTROPIC_COUNT / 4]; Geo_Quad3D quads3D[ENTROPIC_COUNT / 4];
    Geo_Hex2D hexes[ENTROPIC_COUNT / 4]; Geo_HexCone hexesCones[ENTROPIC_COUNT / 4]; Geo_Hex3D hexes3D[ENTROPIC_COUNT / 4];
    Geo_Circle2D circles[ENTROPIC_COUNT / 4]; Geo_CircleCone circlesCones[ENTROPIC_COUNT / 4]; Geo_Circle3D circles3D[ENTROPIC_COUNT / 4];
    std::vector<Geo_Ext2D*> surfaceMeshes; std::vector<Geo_ExtCone*> coneMeshes; std::vector<Geo_Ext3D*> volumeMeshes;
    std::vector<Geo_Surface*> surfacePtMeshes; std::vector<Geo_Cone*> conePtMeshes; std::vector<Geo_Volume*> volumePtMeshes;

    Geo_Actor surface_actors[ENTROPIC_COUNT], conic_actors[ENTROPIC_COUNT], volume_actors[ENTROPIC_COUNT];
    Geo_Actor surfaceExt_actors[ENTROPIC_COUNT], conicExt_actors[ENTROPIC_COUNT], volumeExt_actors[ENTROPIC_COUNT];
    Geo_Actor surfacePt_actors[ENTROPIC_COUNT], conicPt_actors[ENTROPIC_COUNT], volumePt_actors[ENTROPIC_COUNT];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    void setInstances(Geo_Mesh* mesh);
    void createGeometry(unsigned index);
    void createExtGeometry(unsigned index);
    void createPtGeometry(unsigned index);
    void onAnyKey(char key);

    Topl_Scene scenes[ENTROPIC_SCENES] = { PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE, PROGRAM_SCENE };
    // Topl_Scene scene1, scene2, scene3;
    // Topl_Scene ext_scene1, ext_scene2, ext_scene3;
} *Entropy;
