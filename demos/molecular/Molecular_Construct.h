#include "Geo_Construct.hpp"

struct Molecular_Construct : Geo_Construct<Molecular_Construct> {
    Molecular_Construct(unsigned short n) : Geo_Construct("Molecular" + std::to_string(rand() % 999)), nodeCount(n * 3) { init(); }

    void init() override {
        _hub = new Geo_Orb(MOLECULAR_SIZE);
        _hub->drawMode = DRAW_Triangles;
        _geoActors.push_back(Geo_Actor(_hub));

        Vec3f randVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * MOLECULAR_LENGTH;

        for (unsigned n = 0; n < nodeCount; n++) { // Main Nodes
            _orbs.push_back(new Geo_Orb(MOLECULAR_SIZE / (n + 1)));
            _orbs.back()->drawMode = DRAW_Triangles;
            _geoActors.push_back(Geo_Actor(_orbs.back())); // adding  node
            //_geoActors.back().setPos({ (float)rand() / (float)RAND_MAX - 0.5F, (float)rand() / (float)RAND_MAX - 0.5F, 0.0F });
            _geoActors.back().setPos(*_geoActors[n + 1].getPos() + randVec * (MOLECULAR_SIZE / (n + 1)));

            if(n > 1) {
                Geo_Vertex lineVertices[2] = { Geo_Vertex(*_geoActors[n].getPos()), Geo_Vertex(*_geoActors[n + 1].getPos()) };
                _lines.push_back(new Geo_Mesh((vertex_cptr_t)&lineVertices, 2));
                _lines.back()->drawMode = DRAW_Lines;
                _geoActors.push_back(Geo_Actor(_lines.back()));
            }

            randVec = randVec + ((VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * 2);
            // randVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * MOLECULAR_LENGTH;
        }

        for (unsigned n = 1; n < nodeCount; n++) { // Sub Nodes
            Geo_Actor* refActor = &_geoActors[(n * 2) - 1];

            for (unsigned e = 0; e < 3; e++) {
                _orbs.push_back(new Geo_QuadOrb(MOLECULAR_SIZE / (n + 1)));
                _geoActors.push_back(Geo_Actor(_orbs.back()));
            }
        }

        // shift((VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })));
    }

    void configure(Topl_Scene* scene){
        // _physActors.resize(_orbs.size() + 1);
        // _links.resize(_orbs.size());

        // _physActors.front().mass = 10.0F;
        scene->addGeometry(getPrefix() + "hub", &_geoActors[0]);
        // scene->addPhysics(getPrefix() + "hub", &_physActors[0]);
        
        for(unsigned m = 1; m < _orbs.size() + 1; m++) {
            scene->addGeometry(getPrefix() + "node" + std::to_string(m), &_geoActors[m]);
            /* _physActors[m].addForce({10.0F, 10.0F, 10.0F});
            scene->addPhysics(getPrefix() + "node" + std::to_string(m), &_physActors[m]);
            _links[m - 1] = Phys_Connector(*_geoActors[m].getPos(), *_geoActors.front().getPos());
            scene->addLink(&_links[m - 1], getPrefix() + "node" + std::to_string(m), getPrefix() + "hub"); */
        }

        for(unsigned l = _lines.size() + 1; l < _geoActors.size(); l++)
            scene->addGeometry(getPrefix() + "line" + std::to_string(l), &_geoActors[l]);
    }

private:
    const unsigned short nodeCount;

    Geo_Orb* _hub;
    std::vector<Geo_Orb*> _orbs;
    std::vector<Geo_Mesh*> _lines;
    Vec3f _anchor;
};