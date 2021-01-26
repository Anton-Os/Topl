#include "Geo_Construct.hpp"

#include "<utility>"

struct Geo_Grid_Properties {
    Geo_Grid_Properties(){}
	// Uniform sides constructor
	Geo_Grid_Properties(std::pair<unsigned short, float> vals) { 
		widthVals = vals;
		heightVals = vals;
		depthVals = vals;
    }
	// Variable sides constructor
	Geo_Grid_Properties(
		std::pair<unsigned short, float> wv,
		std::pair<unsigned short, float> hv,
		std::pair<unsigned short, float> dv,
	) {
		widthVals = wv;
		heightVals = hv;
		depthVals = dv;
	}
	std::pair<unsigned short, float> widthVals;
	std::pair<unsigned short, float> heightVals;
	std::pair<unsigned short, float> depthVals;
};

class Geo_Grid : public Geo_Construct {
public:
    Geo_Grid(
        const std::string& prefix, 
        Topl_SceneManager* sMan, 
        const Geo_Component* geoComponent, 
        const Geo_Grid_Properties* properties,
		)
    : Geo_Construct(prefix, sMan, geocLink, linkCount){
        grid_prop = *properties;
        
        fillSceneManager(sMan);
    }

    void updateSceneManager(Topl_SceneManager* sMan) override;
	void move(Topl_SceneManager* sMan, Eigen::Vector3f vec) override;
	void rotate(Topl_SceneManager* sMan, Eigen::Vector3f vec) override;
private:
    void fill(Topl_SceneManager* sMan) override;

    std::vector<Phys_Connector> connectors;
    Geo_Grid_Properties grid_prop;
};
