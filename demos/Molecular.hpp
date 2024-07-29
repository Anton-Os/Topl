#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MOLECULAR_CONSTRUCTS 6
#define MOLECULAR_SIZE 0.25F

struct Molecular_Construct : Geo_Construct {
    Molecular_Construct(unsigned short nodeCount) : Geo_Construct("Molecular" + std::to_string(rand() % 999)){
        init(nodeCount);
    }

    void init(unsigned short nodeCount){
        _hub = new Geo_Orb(MOLECULAR_SIZE / 2);
        _geoActors.push_back(Geo_Actor(_hub));
        for(unsigned n = 0; n < nodeCount; n++){
            _orbs.push_back(new Geo_Orb(MOLECULAR_SIZE / nodeCount));
            _geoActors.push_back(Geo_Actor(_orbs.back())); // adding  node
            _geoActors.back().setPos({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F });
        }
    }

    void configure(Topl_Scene* scene){
        _physActors.resize(_orbs.size() + 1);
        _links.resize(_orbs.size());

        _physActors.front().mass = 10.0F;
        scene->addGeometry(getPrefix() + "hub", &_geoActors[0]);
        scene->addPhysics(getPrefix() + "hub", &_physActors[0]);
        // _links[0] = Phys_Connector();
        // scene->addAnchor(&_links[0], getPrefix() + "hub", &_anchor);
        for(unsigned m = 1; m < _orbs.size() + 1; m++) {
            scene->addGeometry(getPrefix() + "node" + std::to_string(m), &_geoActors[m]);
            // _physActors[m] = Phys_Actor();
            _physActors[m].addForce({ 10.0F, 10.0F, 10.0F });
            scene->addPhysics(getPrefix() + "node" + std::to_string(m), &_physActors[m]);
            _links[m - 1] = Phys_Connector(*_geoActors[m].getPos(), *_geoActors.front().getPos());
            scene->addLink(&_links[m - 1], getPrefix() + "node" + std::to_string(m), getPrefix() + "hub");
        }
    }

private:
    Geo_Orb* _hub;
    std::vector<Geo_Orb*> _orbs;
    Vec3f _anchor;
};

struct Molecular_Demo : public Topl_Program {
    Molecular_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Molecular", backend){}

    void init() override;
    void loop(double frameTime) override;

    Molecular_Construct constructs[MOLECULAR_CONSTRUCTS] = {
       Molecular_Construct(2),
       Molecular_Construct(4),
       Molecular_Construct(6),
       Molecular_Construct(8),
       Molecular_Construct(10),
       Molecular_Construct(12)
    };

    Geo_Orb globeMesh = Geo_Orb(5.0F);
    Geo_Actor globe = Geo_Actor(&globeMesh);

    Phys_Connector construct_links[MOLECULAR_CONSTRUCTS - 1];
private:
    Topl_Scene scene;
    Topl_Scene globeScene;
} *_DEMO;