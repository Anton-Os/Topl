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
        Topl_SceneManager* sMan, 
        const Geo_Component* geoLink, 
        const Geo_Chain_Properties* properties,
        unsigned linkCount)
    : Geo_Construct(prefix, sMan, geoLink, linkCount){
		chain_prop = *properties;
        
        fillSceneManager(sMan);
    }

    void updateSceneManager(Topl_SceneManager* sMan) override;
	void move(Topl_SceneManager* sMan, Eigen::Vector3f vec) override;
	void rotate(Topl_SceneManager* sMan, Eigen::Vector3f vec) override;
private:
    void fill(Topl_SceneManager* sMan) override;

    std::vector<Phys_Connector> connectors;
    Geo_Chain_Properties chain_prop;
};
