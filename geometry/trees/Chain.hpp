#include "Geo_Tree.hpp"

struct Geo_Chain_Params {
	Geo_Chain_Params() { directionVec = Vec3f({ 0.1f, 0.0f, 0.0f }); }
    Geo_Chain_Params(Vec3f d) { directionVec = d; }
    
    Vec3f directionVec;
};

class Geo_Chain : public Geo_Tree, public Geo_DynamicSet {
public:
    Geo_Chain( // Prebake Constructor
        const std::string& prefix, 
        const Geo_Actor* geo, 
        const Geo_Chain_Params& props,
        unsigned count)
    : Geo_Tree(prefix, geo, count),
    Geo_DynamicSet(count){
		properties = props;
        origin = Vec3f({
            (props.directionVec.data[0] * count) * -0.5f,
            (props.directionVec.data[1] * count) * -0.5f,
            (props.directionVec.data[2] * count) * -0.5f
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
		properties = props;
        origin = Vec3f({
			(props.directionVec.data[0] * count) * -0.5f,
			(props.directionVec.data[1] * count) * -0.5f,
			(props.directionVec.data[2] * count) * -0.5f
        });
        configure(scene);
    }

    void configure(Topl_Scene* scene) override;
private:
    Vec3f origin; // determines starting position for geometry
    Geo_Chain_Params properties;
};
