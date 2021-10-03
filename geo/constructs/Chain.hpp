#include "Geo_Troupe.hpp"

struct Geo_Chain_Properties {
	Geo_Chain_Properties() { directionVec = Eigen::Vector3f(0.1f, 0.0f, 0.0f); }
    Geo_Chain_Properties(Eigen::Vector3f d) { directionVec = d; }
    
    Eigen::Vector3f directionVec;
};

class Geo_Chain : public Geo_Troupe, public Geo_DynamicSet {
public:
    Geo_Chain(
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Actor* geo, 
        const Geo_Chain_Properties* props,
        unsigned linkCount)
    : Geo_Troupe(prefix, scene, geo, linkCount),
    Geo_DynamicSet(linkCount){
		chain_props = *props;
        
        fillScene(scene);
    }

    void updateScene(Topl_Scene* scene) override;
private:
    void fill(Topl_Scene* scene) override;

    Geo_Chain_Properties chain_props;
};
