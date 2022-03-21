#include "Geo_Tree.hpp"

typedef std::pair<unsigned short, float> gridAttrib_pair;

struct Geo_Grid_Properties {
	Geo_Grid_Properties() {}
	Geo_Grid_Properties(gridAttrib_pair attribs); // Uniform sides constructor
	Geo_Grid_Properties( // Variable sides constructor
		gridAttrib_pair xA,
		gridAttrib_pair yA,
		gridAttrib_pair zA
	);

	void setNonZeroUnits(); // makes sure that all attribute counts are set to non-zero
	unsigned getCellCount() const { return xAttr.first * yAttr.first * zAttr.first; }

	gridAttrib_pair xAttr = std::make_pair(1, 0.0f); // x axis attributes
	gridAttrib_pair yAttr = std::make_pair(1, 0.0f); // y axis attributes
	gridAttrib_pair zAttr = std::make_pair(1, 0.0f); // z axis attributes
};

class Geo_Grid : public Geo_Tree, public Geo_DynamicSet {
public:
	Geo_Grid( // Prebake Constructor
        const std::string& prefix,
        const Geo_Actor* geo, 
        const Geo_Grid_Properties* props
	) : Geo_Tree(prefix, geo, props->getCellCount()),
	Geo_DynamicSet(props->getCellCount()){
        properties = *props;
		origin = Eigen::Vector3f(
			-1.0f * props->xAttr.first * (props->xAttr.second * 0.5) + (props->xAttr.second / 2),
			-1.0f * props->yAttr.first * (props->yAttr.second * 0.5) + (props->yAttr.second / 2),
			0.0f
		);
    }

    Geo_Grid( // Config Constructor
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Actor* geo, 
        const Geo_Grid_Properties* props
	) : Geo_Tree(prefix, geo, props->getCellCount()),
	Geo_DynamicSet(props->getCellCount()){
        properties = *props;
		origin = Eigen::Vector3f(
			-1.0f * props->xAttr.first * (props->xAttr.second * 0.5) + (props->xAttr.second / 2),
			-1.0f * props->yAttr.first * (props->yAttr.second * 0.5) + (props->yAttr.second / 2),
			0.0f
		);
		configure(scene);
    }

	// void blockNode(Geo_Grid_CellIndex cellIndex){ blocks.push_back(cellIndex); } // blocks a node from being displayed
	void configure(Topl_Scene* scene) override;
private:
	Eigen::Vector3f origin; // determines starting position for geometry
    Geo_Grid_Properties properties;
};
