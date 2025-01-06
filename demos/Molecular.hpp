#include "Geo_Construct.hpp"
#include "meshes/Geo_Orbitals.hpp"

#include "program/Topl_Program.hpp"

#define MOLECULAR_CONSTRUCTS 12
#define MOLECULAR_SIZE 0.1F

struct Molecular_Construct : Geo_Construct {
    Molecular_Construct(unsigned short n) : Geo_Construct("Molecular" + std::to_string(rand() % 999)), nodeCount(n) {
        init();
    }

    void init() override {
        _hub = new Geo_Orb(MOLECULAR_SIZE);
        _hub->drawMode = DRAW_Triangles;
        _geoActors.push_back(Geo_Actor(_hub));
        for(unsigned n = 0; n < nodeCount; n++){
            _orbs.push_back(new Geo_Orb(MOLECULAR_SIZE / nodeCount));
            _orbs.back()->drawMode = DRAW_Triangles;
            _geoActors.push_back(Geo_Actor(_orbs.back())); // adding  node
            _geoActors.back().setPos({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F });
        }

        for(unsigned n = 1; n < nodeCount; n++){
            Geo_Vertex lineVertices[2] = { Geo_Vertex(*_geoActors.front().getPos()), Geo_Vertex(*_geoActors[n].getPos()) };
            _lines.push_back(new Geo_Mesh((vertex_cptr_t)&lineVertices, 2));
            _lines.back()->drawMode = DRAW_Lines;
            _geoActors.push_back(Geo_Actor(_lines.back()));
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
            _physActors[m].addForce({ 10.0F, 10.0F, 10.0F });
            scene->addPhysics(getPrefix() + "node" + std::to_string(m), &_physActors[m]);
            _links[m - 1] = Phys_Connector(*_geoActors[m].getPos(), *_geoActors.front().getPos());
            scene->addLink(&_links[m - 1], getPrefix() + "node" + std::to_string(m), getPrefix() + "hub");
        }

        for(unsigned l = _orbs.size() + 1; l < _geoActors.size(); l++)
            scene->addGeometry(getPrefix() + "line" + std::to_string(l), &_geoActors[l]);
    }

private:
    const unsigned short nodeCount;

    Geo_Orb* _hub;
    std::vector<Geo_Orb*> _orbs;
    std::vector<Geo_Mesh*> _lines;
    Vec3f _anchor;
};

struct Molecular_Demo : public Topl_Program {
    Molecular_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Molecular", backend){}

    void init() override;
    void loop(double frameTime) override;

    Molecular_Construct constructs[3][MOLECULAR_CONSTRUCTS] = {
        {
            Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
            Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
            Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10),
            Molecular_Construct(11), Molecular_Construct(12), Molecular_Construct(16)
        },
        {
            Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
            Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
            Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10),
            Molecular_Construct(11), Molecular_Construct(12), Molecular_Construct(16)
        },
        {
            Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
            Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
            Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10),
            Molecular_Construct(11), Molecular_Construct(12), Molecular_Construct(16)
        }
    };

    Phys_Connector construct_links[MOLECULAR_CONSTRUCTS - 1];
private:
    void onAnyKey(char key);

    Topl_Scene scene = PROGRAM_SCENE;
    Topl_Light skyLight = BEAMS_LAMP_LIGHT, flashLight = BEAMS_SKY_LIGHT, lampLight = BEAMS_FLASH_LIGHT;
} *_DEMO;
