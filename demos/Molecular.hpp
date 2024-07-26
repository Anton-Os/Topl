#include "Geo_Construct.hpp"
#include "meshes/Geo_Orb.hpp"

#include "program/Topl_Program.hpp"

#define MOLECULAR_CONSTRUCTS 6

struct Molecular_Construct : Geo_Construct {
    Molecular_Construct(unsigned short nodeCount) : Geo_Construct("Molecular" + std::to_string(rand() % 999)){
        init(nodeCount);
    }

    void init(unsigned short nodeCount){
        _hub = new Geo_Orb();
        _geoActors.push_back(Geo_Actor(_hub));
        for(unsigned n = 0; n < nodeCount; n++){
            _orbs.push_back(new Geo_Orb(1.0 / nodeCount));
            _geoActors.push_back(Geo_Actor(_orbs.back())); // adding  node
            _geoActors.back().setPos({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F });
        }
    }

    void configure(Topl_Scene* scene){
        scene->addGeometry(getPrefix() + "hub", &_geoActors[0]);
        /* _physActors.push_back(Phys_Actor());
        scene->addPhysics(getPrefix() + "hub", &_physActors.back());
        _links.push_back(Phys_Connector());
        scene->addAnchor(&_links.back(), getPrefix() + "hub", &_anchor); */
        for(unsigned m = 1; m < _orbs.size() + 1; m++) {
            scene->addGeometry(getPrefix() + "node" + std::to_string(m), &_geoActors[m]);
            /* _physActors.push_back(Phys_Actor(PHYS_DEFAULT_MASS / _orbs.size()));
            scene->addPhysics(getPrefix() + "node" + std::to_string(m), &_physActors.back());
            _links.push_back(Phys_Connector(*_geoActors.front().getPos(), *_geoActors[m].getPos()));
            scene->addLink(&_links.back(), getPrefix() + "hub", getPrefix() + "node" + std::to_string(m)); */
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
       Molecular_Construct(12),
       Molecular_Construct(16),
       Molecular_Construct(20)
    };

private:
    Topl_Scene scene;
} *_DEMO;