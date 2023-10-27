
#ifndef GEO_LAYOUT_H

#include "meshes/Geo_Shape2D.hpp"

#include "Geo_Grid.hpp"

class Geo_Layout : public Geo_Grid {
	Geo_Layout(unsigned short rows, unsigned short columns) 
	: Geo_Grid("paneLayout", &childActor, Geo_Grid_Params(
		std::make_pair(rows, 0.1F), 
		std::make_pair(columns, 0.1F), 
		std::make_pair(1, 0.0F))
	){
		/* TODO: Add initialization here */
	}

	Geo_Layout(unsigned short rows, unsigned short columns, Topl_Scene* scene)
	: Geo_Grid("paneLayout", &childActor, Geo_Grid_Params(
		std::make_pair(rows, 0.1F), 
		std::make_pair(columns, 0.1F), 
		std::make_pair(1, 0.0F))
	){
		configure(scene);
	}

	void configure(Topl_Scene* scene) override {
		// TODO: Add custom initialization
		scene->addGeometry(getPrefix() + "root", &rootActor);
		Geo_Grid::configure(scene);
	}

	Geo_ShapeQuad childMesh = Geo_ShapeQuad();
	Geo_Actor childActor = Geo_Actor(&childMesh);
	Geo_ShapeQuad rootMesh = Geo_ShapeQuad();
	Geo_Actor rootActor = Geo_Actor(&rootMesh);

	// Rasteron_Image* backgroundImg;
	// Rasteron_MenuItem** menuItems;
};

/* class Geo_VertLayout : Geo_Layout {
    Geo_VertLayout(unsigned short rows) : Geo_Layout(rows, 1){}
    Geo_VertLayout(unsigned short rows, Topl_Scene* scene) : Geo_Layout(rows, 1, scene){}
};

class Geo_HorzLayout : Geo_Layout {
    Geo_HorzLayout(unsigned short cols) : Geo_Layout(1, cols){}
    Geo_HorzLayout(unsigned short cols, Topl_Scene* scene) : Geo_Layout(1, cols, scene){}
};

lass Geo_GridLayout : Geo_Layout {
    Geo_GridLayout(unsigned short num) : Geo_Layout(num, num){}
    Geo_GridLayout(unsigned short num, Topl_Scene* scene) : Geo_Layout(num, num, scene){}
}; */

#define GEO_LAYOUT_H
#endif