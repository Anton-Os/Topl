
#ifndef GEO_LAYOUT_H

#include "meshes/Geo_Shape2D.hpp"

#include "Geo_Grid.hpp"

class Geo_Layout : public Geo_Grid {
public:
	Geo_Layout(unsigned short rows, unsigned short columns) 
	: Geo_Grid("paneLayout", &childMesh, Geo_Grid_Params(
		std::make_pair(rows, 1.0F / rows), 
		std::make_pair(columns, 1.0F / columns), 
		std::make_pair(1, DEFAULT_Z + 0.00001f))
	){ }

	Geo_Layout(unsigned short rows, unsigned short columns, Topl_Scene* scene)
	: Geo_Grid("paneLayout", &childMesh, Geo_Grid_Params(
		std::make_pair(rows, 1.0F / rows), 
		std::make_pair(columns, 1.0F / columns), 
		std::make_pair(1, DEFAULT_Z + 0.00001f))
	){
		configure(scene);
	}

	~Geo_Layout(){
#ifdef RASTERON_H
	free_image(rootBackground);
	for(std::vector<Rasteron_Queue*>::iterator q = childStateBg.begin(); q != childStateBg.end(); q++)
		free_queue(*q);
#endif
	}

	void configure(Topl_Scene* scene) override {
		scene->addGeometry(getPrefix() + "root", &rootActor);
		// childMesh.scale({ 1.25F, 1.25F, 1.25F });
		// childMesh.scale({ 1.0F / _params.x.first, 1.0F / _params.y.first, 0.0F });
		Geo_Grid::configure(scene);
		// TODO: Configure background for root
		// TODO: Configure backgrounds for children
	}
protected:

	Geo_Quad2D childMesh = Geo_Quad2D();
	Geo_Actor childActor = Geo_Actor(&childMesh);
	Geo_Quad2D rootMesh = Geo_Quad2D();
	Geo_Actor rootActor = Geo_Actor(&rootMesh);

#ifdef RASTERON_H
	Rasteron_Image* rootBackground;
	std::vector<Rasteron_Queue*> childStateBg;
#endif
};

struct Geo_VertLayout : Geo_Layout {
    Geo_VertLayout(unsigned short rows) : Geo_Layout(rows, 1){}
    Geo_VertLayout(unsigned short rows, Topl_Scene* scene) : Geo_Layout(rows, 1, scene){}
};

struct Geo_HorzLayout :  Geo_Layout {
    Geo_HorzLayout(unsigned short cols) : Geo_Layout(1, cols){}
    Geo_HorzLayout(unsigned short cols, Topl_Scene* scene) : Geo_Layout(1, cols, scene){}
};

struct Geo_GridLayout : Geo_Layout {
    Geo_GridLayout(unsigned short num) : Geo_Layout(num, num){}
    Geo_GridLayout(unsigned short num, Topl_Scene* scene) : Geo_Layout(num, num, scene){}
};

#define GEO_LAYOUT_H
#endif