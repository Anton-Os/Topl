#include "Geo_Construct.hpp"

static unsigned short constructCount = 0;

struct Molecular_Construct : Geo_Construct<Molecular_Construct> {
    Molecular_Construct(unsigned short n) : Geo_Construct("Molecular" + std::to_string(constructCount + 1)), nodeCount(n), subnodeCount(n) { 
        constructCount++;
        init(); 
    }

    Molecular_Construct(unsigned short n, unsigned short s) : Geo_Construct("Molecular" + std::to_string(constructCount + 1)), nodeCount(n), subnodeCount(s) {
        constructCount++;
        init();
    }

    void init() override {
        _hub = new Geo_Orb(MOLECULAR_SIZE);
        _hub->drawMode = DRAW_Points;
        _geoActors.push_back(Geo_Actor(getPrefix() + "hub", _hub));

        Vec3f randVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * MOLECULAR_LENGTH;

        for (unsigned n = 0; n < nodeCount; n++) { // Main Nodes
            _orbs.push_back(new Geo_Orb(MOLECULAR_SIZE / 4));
            _orbs.back()->drawMode = DRAW_Triangles;
            _geoActors.push_back(Geo_Actor(getPrefix() + "node" + std::to_string(n + 1), _orbs.back())); // adding  node
            //_geoActors.back().setPos({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F });
            _geoActors.back().setPos(*_geoActors[n + 1].getPos() + randVec);

            Geo_Vertex lineVertices[2] = { Geo_Vertex(*_geoActors[n].getPos()), Geo_Vertex(*_geoActors[n + 1].getPos()) };
            _lines.push_back(new Geo_Mesh((vertex_cptr_t)&lineVertices, 2));
            _lines.back()->drawMode = DRAW_Lines;
            _geoActors.push_back(Geo_Actor(getPrefix() + "line" + std::to_string(n + 1), _lines.back()));

            randVec = randVec + ((VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * 2) * MOLECULAR_LENGTH;
            // randVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * MOLECULAR_LENGTH;
        }

        for (unsigned n = 1; n < nodeCount; n++) { // Sub Nodes
            Geo_Actor* refActor = &_geoActors[(n * 2) - 1];
            _orbs.push_back(new Geo_Orb(create_shape3D(MOLECULAR_SIZE / 6, n + 2, n + 2)));
            _orbs.back()->drawMode = DRAW_Fan;

            for (unsigned s = 0; s < subnodeCount; s++) {
                // _orbs.push_back(new Geo_QuadOrb(MOLECULAR_SIZE / (n + 1)));
                _geoActors.push_back(Geo_Actor(getPrefix() + "subnode" + std::to_string(n + 1) + "." + std::to_string(s + 1), _orbs.back()));
                _geoActors.back().setPos(*refActor->getPos() + ((VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * 2) * MOLECULAR_LENGTH * 0.25);

                Geo_Vertex lineVertices[2] = { Geo_Vertex(*refActor->getPos()), Geo_Vertex(*_geoActors.back().getPos())};
                _lines.push_back(new Geo_Mesh((vertex_cptr_t)&lineVertices, 2));
                _lines.back()->drawMode = DRAW_Lines;
                _geoActors.push_back(Geo_Actor(getPrefix() + "subline" + std::to_string(n + 1) + "." + std::to_string(s + 1), _lines.back()));
            }
        }

        // shift((VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })));
    }

    void configure(Topl_Scene* scene){
        _physActors.resize((_geoActors.size() / 2) + 1);
        // _links.resize(_orbs.size());

        // _physActors.front().mass = 10.0F;
        scene->addGeometry(&_geoActors[0]);
        scene->addPhysics(_geoActors[0].getName(), &_physActors[0]);
        
        for(unsigned m = 1; m < _geoActors.size(); m++) {
            scene->addGeometry(&_geoActors[m]);
            /* if (_geoActors[m].getName().find("line") == std::string::npos) {
                // _physActors.push_back(Phys_Actor());
                scene->addPhysics(_geoActors[m].getName(), &_physActors[m]);
                if(_geoActors[m].getName().find("sub") == std::string::npos) {
                    _links.push_back(Phys_Connector(*_geoActors[m].getPos(), *_geoActors.front().getPos()));
                    scene->addLink(&_links[m - 1], getPrefix() + "node" + std::to_string(m), getPrefix() + "hub");
                } 
            } */
        }
    }

    void addForce(Vec3f force) { _physActors[0].addForce(force); }

private:
    const unsigned short nodeCount;
    const unsigned short subnodeCount;

    Geo_Orb* _hub;
    std::vector<Geo_Orb*> _orbs;
    std::vector<Geo_Mesh*> _lines;
    Vec3f _anchor;
};