#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "Geo_Construct.hpp"

#include "program/Topl_Program.hpp"

#define MESHSCAPE_SIZE 0.5F
#define MESHSCAPE_BRUSHES 4
#define MESHSCAPE_NAME "meshscape" + std::to_string(rand() % 9999)

struct Meshscape_Construct : public Geo_Construct {
    Meshscape_Construct(Geo_Mesh* m, std::initializer_list<Vec3f> points) : Geo_Construct(MESHSCAPE_NAME), brushMesh(m){ 
        for(auto p = points.begin(); p != points.end(); p++) brushPoints.push_back(*p);
        init(); 
    }

    void init() override {
        for(auto b = brushPoints.begin(); b != brushPoints.end(); b++){
            _geoActors.push_back(Geo_Actor(brushMesh));
            _geoActors.back().setPos(*b);
        }
    }

    void configure(Topl_Scene* scene) override {
        // for(unsigned g = 0; g < _geoActors.size(); g++) std::cout << "Adding geometry for " << getPrefix() + "mesh" + std::to_string(g + 1) << std::endl;
        for(unsigned g = 0; g < _geoActors.size(); g++) scene->addGeometry(getPrefix() + "mesh" + std::to_string(g + 1), &_geoActors[g]);
    }

    Geo_Mesh* getCompositeMesh(); // Return new mesh with sum of actor meshes
private:
    const Geo_Mesh *const brushMesh;
    Geo_Mesh compositeMesh;

    std::vector<Vec3f> brushPoints;
};

struct Meshscape_Demo : public Topl_Program {
    Meshscape_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Meshscape", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    void onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    unsigned short sculptMode;

    Topl_Scene scene = PROGRAM_SCENE;
    Topl_Scene sculptScene = PROGRAM_SCENE;

    Geo_Mesh brushMeshes[MESHSCAPE_BRUSHES] = {
        Geo_Trig3D(MESHSCAPE_SIZE), Geo_Quad3D(MESHSCAPE_SIZE), Geo_Hex3D(MESHSCAPE_SIZE), Geo_Circle3D(MESHSCAPE_SIZE)
    };

    std::vector<Meshscape_Construct> constructs;
    std::vector<Geo_Actor*> sculptActors;
} *Meshscape;