
#ifndef GEO_LAYOUT_H

#include "meshes/Geo_Shape2D.hpp"

#include "Geo_Grid.hpp"

#define PANE_SIZE 0.5F
#define PANE_Z 0.0F

class Geo_Layout : public Geo_Grid {
public:
	Geo_Layout(const std::string& prefix, unsigned short rows, unsigned short columns) 
	: Geo_Grid(prefix, &childMesh, Geo_Grid_Params(
		std::make_pair(rows, PANE_SIZE / rows), 
		std::make_pair(columns, PANE_SIZE / columns), 
		std::make_pair(1, 0.0F))
	){ }

	Geo_Layout(const std::string& prefix, unsigned short rows, unsigned short columns, Topl_Scene* scene)
	: Geo_Grid(prefix, &childMesh, Geo_Grid_Params(
		std::make_pair(rows, PANE_SIZE / rows), 
		std::make_pair(columns, PANE_SIZE / columns), 
		std::make_pair(1, 0.0F))
	){
		configure(scene);
	}

	~Geo_Layout(){
#ifdef RASTERON_H
	/* dealloc_image(rootBk);
	for(std::vector<Rasteron_Queue*>::iterator q = childStateBg.begin(); q != childStateBg.end(); q++)
		free_queue(*q); */
#endif
	}

	void shift(Vec3f vec){ // TODO: Include root actor as part of _geoActors
		Geo_Construct::shift(vec);
		rootActor.updatePos(vec);
    }

	void configure(Topl_Scene* scene) override {
		// childMesh.scale({ 1.25F, 1.25F, 1.25F });
		childMesh.scale({ 1.0F / _params.x.first, 1.0F / _params.y.first, 0.0F });
		Geo_Grid::configure(scene);

		// _geoActors.resize(rootActor); // TODO: Include as part of geoActors
		if(rootBorder > 0.0F) rootMesh.scale({ 1.0F + rootBorder, 1.0F + rootBorder, 0.0F });
		scene->addGeometry(getPrefix() + "root", &rootActor);
#ifdef RASTERON_H
		// TODO: Add texturing commands
#endif
	}
protected:
	Geo_Quad2D childMesh = Geo_Quad2D(PANE_SIZE, PANE_Z + 0.0001F);
	// Geo_Actor childActor = Geo_Actor(&childMesh);
	float rootBorder = 0.05F;
	Geo_Quad2D rootMesh = Geo_Quad2D(PANE_SIZE, PANE_Z);
	Geo_Actor rootActor = Geo_Actor(&rootMesh);

#ifdef RASTERON_H
	Img_Base* rootBk; // root background
	std::map<const Geo_Actor*, Img_Array*> childBks; // child backgrounds
	// std::vector<Rasteron_Queue*> childStateBg;
#endif
};

struct Geo_VertLayout : Geo_Layout {
    Geo_VertLayout(const std::string& prefix, unsigned short rows) : Geo_Layout(prefix, rows, 1){}
    Geo_VertLayout(const std::string& prefix, unsigned short rows, Topl_Scene* scene) : Geo_Layout(prefix, rows, 1, scene){}
};

struct Geo_HorzLayout :  Geo_Layout {
    Geo_HorzLayout(const std::string& prefix, unsigned short cols) : Geo_Layout(prefix, 1, cols){}
    Geo_HorzLayout(const std::string& prefix, unsigned short cols, Topl_Scene* scene) : Geo_Layout(prefix, 1, cols, scene){}
};

struct Geo_GridLayout : Geo_Layout {
    Geo_GridLayout(const std::string& prefix, unsigned short num) : Geo_Layout(prefix, num, num){}
    Geo_GridLayout(const std::string& prefix,unsigned short num, Topl_Scene* scene) : Geo_Layout(prefix, num, num, scene){}
};

#define GEO_LAYOUT_H
#endif