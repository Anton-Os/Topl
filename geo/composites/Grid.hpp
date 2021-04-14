#include "Geo_Construct.hpp"

// #include "<utility>"

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
		std::pair<unsigned short, float> dv
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
        Topl_Scene* scene, 
        const Geo_Component* geoComponent, 
        const Geo_Grid_Properties* properties
		)
    : Geo_Construct(
		prefix, 
		scene, 
		geoComponent, 
		properties->widthVals.first * properties->heightVals.first * properties->depthVals.first){ // Requires the volume

        grid_prop = *properties;
		bottomLeftCorner = Eigen::Vector3f(
			-1.0f * properties->widthVals.first * (properties->widthVals.second * 0.5), // inverse of count times distance halved of each cell
			-1.0f * properties->heightVals.first * (properties->heightVals.second * 0.5), // inverse of count times distance halved of each cell
			0.0f // TODO: Change this to non hard-coded
		);
        
        fillSceneManager(scene);
    }

    void updateSceneManager(Topl_Scene* scene) override;
private:
    void fill(Topl_Scene* scene) override;

    std::vector<Phys_Connector> connectors;
	Eigen::Vector3f bottomLeftCorner; // Use this to determine grid item position
    Geo_Grid_Properties grid_prop;
};
