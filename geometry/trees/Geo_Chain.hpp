#include "Geo_Tree.hpp"

// Parameters

struct Geo_Chain_Params {
	Geo_Chain_Params() { direction = Vec3f({ 0.1f, 0.0f, 0.0f }); }
    Geo_Chain_Params(Vec3f d) { direction = d; }
    
    Vec3f direction;
};

// Geometry Sequence

class Geo_Chain : public Geo_Tree, public Geo_DynamicSet {
public:
    Geo_Chain( // Prebake Constructor
        const std::string& prefix, 
        const Geo_Actor* geo, 
        const Geo_Chain_Params& params,
        unsigned count)
    : Geo_Tree(prefix, geo, count),
    Geo_DynamicSet(count){
		_params = params;
        _origin = Vec3f({
            (params.direction.data[0] * count) * -0.5f,
            (params.direction.data[1] * count) * -0.5f,
            (params.direction.data[2] * count) * -0.5f
        });
    }

    Geo_Chain( // Config Constructor
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Actor* geo, 
        const Geo_Chain_Params& props,
        unsigned count)
    : Geo_Tree(prefix, geo, count),
    Geo_DynamicSet(count){
		_params = props;
        _origin = Vec3f({
			(props.direction.data[0] * count) * -0.5f,
			(props.direction.data[1] * count) * -0.5f,
			(props.direction.data[2] * count) * -0.5f
        });
        configure(scene);
    }

    void configure(Topl_Scene* scene) override;
private:
    Vec3f _origin; // determines starting position for geometry
    Geo_Chain_Params _params;
};
