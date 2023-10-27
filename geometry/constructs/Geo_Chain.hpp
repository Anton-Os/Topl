#ifndef GEO_CHAIN_H

#include "Geo_Construct.hpp"

// Geometry Chain

class Geo_Chain : public Geo_Construct {
public:
    Geo_Chain(const std::string& prefix, const Geo_Actor* actor, Vec3f direction, unsigned count)
    : Geo_Construct(prefix, actor, count){ // Non-configured Constructor
		_direction = direction;
        _origin = Vec3f({
			(_direction.data[0] * count) * -0.5f,
			(_direction.data[1] * count) * -0.5f,
			(_direction.data[2] * count) * -0.5f
        });
    }

    Geo_Chain(const std::string& prefix, Topl_Scene* scene, const Geo_Actor* actor, Vec3f direction, unsigned count)
    : Geo_Construct(prefix, actor, count) { // Configured Constructor
		_direction = direction;
        _origin = Vec3f({
			(_direction.data[0] * count) * -0.5f,
			(_direction.data[1] * count) * -0.5f,
			(_direction.data[2] * count) * -0.5f
        });
        configure(scene);
    }

    static std::string genLinkName(unsigned num){ return "link" + std::to_string(num); }

    void configure(Topl_Scene* scene) override {
        for(unsigned c = 0; c < _geoActors.size(); c++){
            _geoActors[c].updatePos(_origin + (_direction * c));
            scene->addGeometry(getPrefix() + genLinkName(c + 1), &_geoActors[c]);
            scene->addPhysics(getPrefix() + genLinkName(c + 1), &_physActors.at(c));

            if(c > 0){
                _links.push_back(Phys_Connector());
                scene->addLink(&_links.back(), getPrefix() + genLinkName(c), getPrefix() + genLinkName(c + 1));
            }
        }
    }
private:
    Vec3f _direction = Vec3f({ 0.1f, 0.0f, 0.0f });
};

#define GEO_CHAIN_H
#endif