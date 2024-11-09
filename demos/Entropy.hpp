#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Iterable.hpp"

#include "program/Topl_Program.hpp"

#define ENTROPIC_TRIG 0
#define ENTROPIC_QUAD 1
#define ENTROPIC_HEX 2
#define ENTROPIC_CIRCLE 3

#define ENTROPIC_ITERS 4
#define ENTROPIC_SPAWN 100
#define ENTROPIC_COUNT 200
#define ENTROPIC_PROB 0.05
#define ENTROPIC_SIZE 0.045
#define ENTROPIC_FORCE 50000.0F

struct Entropy_Demo : public Topl_Program {
    Entropy_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Entropy", backend){
        backdropActor.setPos({ 0.0F, 0.0F, -0.1F });
        for(unsigned a = 0; a < ENTROPIC_COUNT; a++){
            switch(a % 4){
                case ENTROPIC_TRIG: surface_actors[a] = Geo_Actor(&trigs[a / 4]); conic_actors[a] = Geo_Actor(&trigsCones[a / 4]); volume_actors[a] = Geo_Actor(&trigs3D[a / 4]); break;
                case ENTROPIC_QUAD: surface_actors[a] = Geo_Actor(&quads[a / 4]); conic_actors[a] = Geo_Actor(&quadsCones[a / 4]); volume_actors[a] = Geo_Actor(&quads3D[a / 4]); break;
                case ENTROPIC_HEX: surface_actors[a] = Geo_Actor(&hexes[a / 4]); conic_actors[a] = Geo_Actor(&hexesCones[a / 4]); volume_actors[a] = Geo_Actor(&hexes3D[a / 4]); break;
                case ENTROPIC_CIRCLE: surface_actors[a] = Geo_Actor(&circles[a / 4]); conic_actors[a] = Geo_Actor(&circlesCones[a / 4]); volume_actors[a] = Geo_Actor(&circles3D[a / 4]); break;
            }

            surface_actors[a].setName("actor_surface" + std::to_string(a));
            surface_actors[a].setPos({ (float)rand() / (float)RAND_MAX - 0.5f, (float)rand() / (float)RAND_MAX - 0.5f, 0.0 });
            surface_actors[a].setRot({ (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, });
            float size = (float)rand() / (float)RAND_MAX * ENTROPIC_SIZE;
            surface_actors[a].setSize({ (size > 0.01F)? size : 0.01F, (size > 0.01F)? size : 0.01F, (size > 0.01F)? size : 0.01F });
            
            // Extended Geometry & Actors

            switch(a % 4){
                case ENTROPIC_TRIG: 
                    surfaceMeshes.push_back(new Geo_Ext2D({ trigs[a / 4].getRadius(), trigs[a / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
                    coneMeshes.push_back(new Geo_ExtCone({ trigs[a / 4].getRadius(), trigs[a / 4].getSegments() }, trigsCones[a / 4].getApex(), ENTROPIC_ITERS));
                    volumeMeshes.push_back(new Geo_Ext3D({ trigs[a / 4].getRadius(), trigs[a / 4].getSegments() }, trigs3D[a / 4].getDepth(), ENTROPIC_ITERS));
                break;
                case ENTROPIC_QUAD: 
                    surfaceMeshes.push_back(new Geo_Ext2D({ quads[a / 4].getRadius(), quads[a / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
                    coneMeshes.push_back(new Geo_ExtCone({ quads[a / 4].getRadius(), quads[a / 4].getSegments() }, quadsCones[a / 4].getApex(), ENTROPIC_ITERS));
                    volumeMeshes.push_back(new Geo_Ext3D({ quads[a / 4].getRadius(), quads[a / 4].getSegments() }, quads3D[a / 4].getDepth(), ENTROPIC_ITERS));
                break;
                case ENTROPIC_HEX: 
                    surfaceMeshes.push_back(new Geo_Ext2D({ hexes[a / 4].getRadius(), hexes[a / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
                    coneMeshes.push_back(new Geo_ExtCone({ hexes[a / 4].getRadius(), hexes[a / 4].getSegments() }, hexesCones[a / 4].getApex(), ENTROPIC_ITERS));
                    volumeMeshes.push_back(new Geo_Ext3D({ hexes[a / 4].getRadius(), hexes[a / 4].getSegments() }, hexes3D[a / 4].getDepth(), ENTROPIC_ITERS));
                break;
                case ENTROPIC_CIRCLE: 
                    surfaceMeshes.push_back(new Geo_Ext2D({ circles[a / 4].getRadius(), circles[a / 4].getSegments() }, DEFAULT_Z, ENTROPIC_ITERS));
                    coneMeshes.push_back(new Geo_ExtCone({ circles[a / 4].getRadius(), circles[a / 4].getSegments() }, circlesCones[a / 4].getApex(), ENTROPIC_ITERS));
                    volumeMeshes.push_back(new Geo_Ext3D({ circles[a / 4].getRadius(), circles[a / 4].getSegments() }, circles3D[a / 4].getDepth(), ENTROPIC_ITERS));
                break;
            }

            // surfaceMeshes[a]->rotate({ 0.1F * (a + 1), 0.1F * (a + 1), 0.1F * (a + 1) });

            surfaceExt_actors[a] = Geo_Actor(surfaceMeshes.back());
            surfaceExt_actors[a].setPropsTo(surface_actors[a]);
            conicExt_actors[a] = Geo_Actor(coneMeshes.back());
            conicExt_actors[a].setPropsTo(conic_actors[a]);
            volumeExt_actors[a] = Geo_Actor(volumeMeshes.back());
            volumeExt_actors[a].setPropsTo(volume_actors[a]);
        }
    }

    void init() override;
    void loop(double frameTime) override;

    static unsigned short mode;

    Topl_Scene* getScene(){
        // _renderer->setDrawMode((mode == 0 || mode == 3)? DRAW_Triangles : DRAW_Points);

        switch(mode % 6){
            case 1: return &scene2;
            case 2: return &scene3;
            case 3: return &ext_scene1;
            case 4: return &ext_scene2;
            case 5: return &ext_scene3;
            default: return &scene1;
        }
    }

    Geo_Quad2D* backdropMesh = new Geo_Quad2D(100.0);
    Geo_Actor backdropActor = Geo_Actor(backdropMesh);

    Geo_Trig2D trigs[ENTROPIC_COUNT / 4]; Geo_TrigCone trigsCones[ENTROPIC_COUNT / 4]; Geo_Trig3D trigs3D[ENTROPIC_COUNT / 4];
    Geo_Quad2D quads[ENTROPIC_COUNT / 4]; Geo_QuadCone quadsCones[ENTROPIC_COUNT / 4]; Geo_Quad3D quads3D[ENTROPIC_COUNT / 4];
    Geo_Hex2D hexes[ENTROPIC_COUNT / 4]; Geo_HexCone hexesCones[ENTROPIC_COUNT / 4]; Geo_Hex3D hexes3D[ENTROPIC_COUNT / 4];
    Geo_Circle2D circles[ENTROPIC_COUNT / 4]; Geo_CircleCone circlesCones[ENTROPIC_COUNT / 4]; Geo_Circle3D circles3D[ENTROPIC_COUNT / 4];
    // Geo_Actor actors[ENTROPIC_COUNT];
    Geo_Actor surface_actors[ENTROPIC_COUNT]; Geo_Actor conic_actors[ENTROPIC_COUNT]; Geo_Actor volume_actors[ENTROPIC_COUNT];

    std::vector<Geo_Ext2D*> surfaceMeshes; std::vector<Geo_ExtCone*> coneMeshes; std::vector<Geo_Ext3D*> volumeMeshes;
    Geo_Actor surfaceExt_actors[ENTROPIC_COUNT]; Geo_Actor conicExt_actors[ENTROPIC_COUNT]; Geo_Actor volumeExt_actors[ENTROPIC_COUNT];
#ifdef TOPL_ENABLE_PHYSICS
    Phys_Actor physActors[ENTROPIC_COUNT];
#endif
private:
    Topl_Scene scene1, scene2, scene3;
    Topl_Scene ext_scene1, ext_scene2, ext_scene3;
    Topl_Scene backdropScene;
    // Topl_Scene spawnScene;
} *_DEMO;
