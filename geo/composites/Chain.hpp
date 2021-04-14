#include "Geo_Construct.hpp"

struct Geo_Chain_Properties {
    Geo_Chain_Properties(){}
	Geo_Chain_Properties(float d) { 
        distance = d;
        // TODO: Add more properties
    }
	float distance = 0.1f;
};

class Geo_Chain : public Geo_Construct {
public:
    Geo_Chain(
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Component* geoLink, 
        const Geo_Chain_Properties* properties,
        unsigned linkCount)
    : Geo_Construct(prefix, scene, geoLink, linkCount){
		chain_prop = *properties;
        
        fillscene(scene);
    }

    void updateScene(Topl_Scene* scene) override;
private:
    void fill(Topl_Scene* scene) override;

    std::vector<Phys_Connector> connectors;
    Geo_Chain_Properties chain_prop;
};
