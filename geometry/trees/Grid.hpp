#include "Geo_Tree.hpp"

/* struct Geo_Grid_CellIndex{
	unsigned xOffset;
	unsigned yOffset;
	unsigned zOffset;
}; */

struct Geo_Grid_Properties {
	Geo_Grid_Properties() {}
	Geo_Grid_Properties(std::pair<unsigned short, float> attribs) { // Uniform sides constructor
		xAttr = attribs;
		yAttr = attribs;
		zAttr = attribs;
		setNonZeroUnits();
    }
	Geo_Grid_Properties( // Variable sides constructor
		std::pair<unsigned short, float> xa,
		std::pair<unsigned short, float> ya,
		std::pair<unsigned short, float> za
	) {
		xAttr = xa;
		yAttr = ya;
		zAttr = za;
		setNonZeroUnits();
	}

	void setNonZeroUnits(){ // makes sure that all counts are set tonon-zero
		if(xAttr.first == 0) xAttr.first++;
		if(yAttr.first == 0) yAttr.first++;
		if(zAttr.first == 0) zAttr.first++;
	}

	unsigned getCellCount() const {
		return xAttr.first * yAttr.first * zAttr.first;
	}

	std::pair<unsigned short, float> xAttr; // unit count and spacing along x axis
	std::pair<unsigned short, float> yAttr; // unit count and spacing along y axis
	std::pair<unsigned short, float> zAttr; // unit count and spacing along z axis
};

class Geo_Grid : public Geo_Tree, public Geo_DynamicSet {
public:
    Geo_Grid(
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Actor* geo, 
        const Geo_Grid_Properties* props
	) : Geo_Tree(prefix, scene, geo, props->getCellCount()),
	Geo_DynamicSet(props->getCellCount()){
        properties = *props;
		origin = Eigen::Vector3f(
			-1.0f * props->xAttr.first * (props->xAttr.second * 0.5) + (props->xAttr.second / 2),
			-1.0f * props->yAttr.first * (props->yAttr.second * 0.5) + (props->yAttr.second / 2),
			0.0f
		);

		fill(scene);
    }

	// void blockNode(Geo_Grid_CellIndex cellIndex){ blocks.push_back(cellIndex); } // blocks a node from being displayed
private:
    void fill(Topl_Scene* scene) override;
	
	Eigen::Vector3f origin; // determines starting position for geometry
	// std::vector<Geo_Grid_CellIndex> blocks; // blocked cells are not rendered to screen 
    Geo_Grid_Properties properties;
};
