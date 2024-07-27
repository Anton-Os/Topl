#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MOLECULAR_CONSTRUCTS 3
#define MOLECULAR_SIZE 0.25F

struct Molecular_Construct : Geo_Construct {
    Molecular_Construct(unsigned short nodeCount) : Geo_Construct("Molecular" + std::to_string(rand() % 999)){
        init(nodeCount);
    }

    void init(unsigned short nodeCount){
        _hub = new Geo_Orb(MOLECULAR_SIZE);
        _geoActors.push_back(Geo_Actor(_hub));
        for(unsigned n = 0; n < nodeCount; n++){
            _orbs.push_back(new Geo_Orb(MOLECULAR_SIZE/ nodeCount));
            _geoActors.push_back(Geo_Actor(_orbs.back())); // adding  node
            _geoActors.back().setPos({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F });
        }
    }

    void configure(Topl_Scene* scene){
        _physActors.resize(_orbs.size() + 1);
        _links.resize(_orbs.size() + 1);
        scene->addGeometry(getPrefix() + "hub", &_geoActors[0]);
        scene->addPhysics(getPrefix() + "hub", &_physActors[0]);
        _links[0] = Phys_Connector();
        scene->addAnchor(&_links[0], getPrefix() + "hub", &_anchor);
        for(unsigned m = 1; m < _orbs.size() + 1; m++) {
            scene->addGeometry(getPrefix() + "node" + std::to_string(m), &_geoActors[m]);
            _physActors[m] = Phys_Actor(PHYS_DEFAULT_MASS / _orbs.size());
            scene->addPhysics(getPrefix() + "node" + std::to_string(m), &_physActors[m]);
            _links[m] = Phys_Connector(*_geoActors.front().getPos(), *_geoActors[m].getPos());
            scene->addLink(&_links[m], getPrefix() + "hub", getPrefix() + "node" + std::to_string(m));
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
       Molecular_Construct(3),
       Molecular_Construct(5),
       Molecular_Construct(8),
       /* Molecular_Construct(12),
       Molecular_Construct(16),
       Molecular_Construct(20) */
    };

    Phys_Connector construct_links[MOLECULAR_CONSTRUCTS - 1];
private:
    Topl_Scene scene;
} *_DEMO;