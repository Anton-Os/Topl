#ifndef GEO_CHAIN_H

#include "Geo_Construct.hpp"

// Geometry Chain

class Geo_Chain : public Geo_Construct {
public:
    Geo_Chain(const std::string& prefix, const Geo_Mesh* mesh, Vec3f direction, unsigned count)
    : Geo_Construct(prefix, mesh, count){ // Non-configured Constructor
		_direction = direction;
        _origin = Vec3f({
			(_direction.data[0] * count) * -0.5f,
			(_direction.data[1] * count) * -0.5f,
			(_direction.data[2] * count) * -0.5f
        });
        init();
    }

    Geo_Chain(const std::string& prefix, Topl_Scene* scene, const Geo_Mesh* mesh, Vec3f direction, unsigned count)
    : Geo_Construct(prefix, mesh, count) { // Configured Constructor
		_direction = direction;
        _origin = Vec3f({
			(_direction.data[0] * count) * -0.5f,
			(_direction.data[1] * count) * -0.5f,
			(_direction.data[2] * count) * -0.5f
        });
        init();
        configure(scene);
    }

    std::string getLinkName(unsigned num){ return getPrefix() + "link" + std::to_string(num); }

    void init() override {
        for(unsigned c = 0; c < _geoActors.size(); c++) _geoActors[c].updatePos(_origin + (_direction * c));
#ifdef TOPL_ENABLE_PHYSICS
        _links.resize(_geoActors.size() - 1);
#endif
    }

    void configure(Topl_Scene* scene) override {
        for(unsigned c = 0; c < _geoActors.size(); c++){
            scene->addGeometry(getLinkName(c + 1), &_geoActors[c]);
#ifdef TOPL_ENABLE_PHYSICS
            scene->addPhysics(getLinkName(c + 1), &_physActors.at(c));
            if(c > 0) scene->addLink(&_links[c - 1], getLinkName(c), getLinkName(c + 1));
#endif
        }
    }
private:
    Vec3f _direction = Vec3f({ 0.1f, 0.0f, 0.0f });
};

#define GEO_CHAIN_H
#endif
