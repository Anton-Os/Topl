
#ifndef GEO_LAYOUT_H

#include "meshes/Geo_Surface.hpp"

#include "Geo_Grid.hpp"

#define PANE_SIZE 0.5F
#define PANE_Z 0.0F

class Geo_Billboard : public Geo_Grid {
public:
	Geo_Billboard(const std::string& prefix, unsigned short rows, unsigned short columns) 
	: Geo_Grid(prefix, &childMesh, Geo_Grid_Params(
		std::make_pair(rows, PANE_SIZE / rows), 
		std::make_pair(columns, PANE_SIZE / columns), 
		std::make_pair(1, 0.0F))
	){ }

	Geo_Billboard(const std::string& prefix, unsigned short rows, unsigned short columns, Topl_Scene* scene)
	: Geo_Grid(prefix, &childMesh, Geo_Grid_Params(
		std::make_pair(rows, PANE_SIZE / rows), 
		std::make_pair(columns, PANE_SIZE / columns), 
		std::make_pair(1, 0.0F))
	){
		configure(scene);
	}


	void configure(Topl_Scene* scene) override {
		_geoActors.resize(_params.getGridSize() + 1);
		if(rootBorder > 0.0F) rootMesh.scale({ 1.0F + rootBorder, 1.0F + rootBorder, 0.0F });
		_geoActors[_params.getGridSize()] = Geo_Actor(&rootMesh);

		// childMesh.scale({ 1.25F, 1.25F, 1.25F });
		childMesh.scale({ 1.0F / _params.x.first, 1.0F / _params.y.first, 0.0F });
		Geo_Grid::configure(scene);

		scene->addGeometry(getPrefix() + "root", &_geoActors.back()); 
		// _geoActors.back().pickerFunc = onPick;
		// for(unsigned p = 0; p < _params.getGridSize(); p++) _geoActors.at(p).pickerFunc = onPick;
#ifdef RASTERON_H
		for(unsigned p = 0; p < _params.getGridSize(); p++){
			paneImg_map.insert({ &_geoActors.at(p), Img_Base(0xFF666666) });
			scene->addTexture(getCellName(p + 1), &paneImg_map.at(&_geoActors.at(p))); 
		}
		scene->addTexture(getPrefix() + "root", &rootImg);
#endif
	}
	void scale(Vec3f scaleVec){
        for(unsigned g = 0; g < _geoActors.size(); g++) {
			_geoActors[g].setSize(scaleVec);
			Vec3f offsetVec = *_geoActors[_params.getGridSize()].getPos() - *_geoActors[g].getPos();
			if(_params.getGridSize() != g)
				_geoActors[g].updatePos({ offsetVec[0] * scaleVec[0] * 1.5F, offsetVec[1] * scaleVec[1] * 1.5F, 0.0F });
		}
    }

	Img_Base* getImgAt(unsigned short i){ return (i != _params.getGridSize())? &paneImg_map.at(&_geoActors.at(i)) : &rootImg; }

	void overlay(unsigned paneIndex, Img_UI* element){ getImgAt(paneIndex)->setImage(element->stateImg.getImage()); }
	/* void overlay(unsigned paneIndex, Img_Button* button){ getImgAt(paneIndex)->setImage(button->stateImg.getImage()); }
	void overlay(unsigned paneIndex, Img_Label* label){ getImgAt(paneIndex)->setImage(label->stateImg.getImage()); }
	void overlay(unsigned paneIndex, Img_Dial* dial){ getImgAt(paneIndex)->setImage(dial->stateImg.getImage()); }
	void overlay(unsigned paneIndex, Img_Slider* slider){ getImgAt(paneIndex)->setImage(slider->stateImg.getImage()); } */
protected:
	Geo_Quad2D childMesh = Geo_Quad2D(PANE_SIZE, PANE_Z + 0.0001F);
	// Geo_Actor childActor = Geo_Actor(&childMesh);
	float rootBorder = 0.05F;
	Geo_Quad2D rootMesh = Geo_Quad2D(PANE_SIZE, PANE_Z);
	Geo_Actor rootActor = Geo_Actor(&rootMesh);
#ifdef RASTERON_H
	Img_Base rootImg = Img_Base(0xFF333333); // root background
	std::map<const Geo_Actor*, Img_Base> paneImg_map; // for child images
	// std::map<const Geo_Actor*, Img_UI> paneImgUI_map; // for child UI elements;
	// std::map<const Geo_Actor*, pickerCallback> panesOnPick_map;
#endif
};

struct Geo_Paneboard : Geo_Billboard {
	Geo_Paneboard(const std::string& prefix) : Geo_Billboard(prefix, 1, 1){}
    Geo_Paneboard(const std::string& prefix, Topl_Scene* scene) : Geo_Billboard(prefix, 1, 1, scene){}
};

struct Geo_Crossboard : Geo_Billboard {
    Geo_Crossboard(const std::string& prefix, unsigned short rows) : Geo_Billboard(prefix, rows, 1){}
    Geo_Crossboard(const std::string& prefix, unsigned short rows, Topl_Scene* scene) : Geo_Billboard(prefix, rows, 1, scene){}
};

struct Geo_Listboard :  Geo_Billboard {
    Geo_Listboard(const std::string& prefix, unsigned short cols) : Geo_Billboard(prefix, 1, cols){}
    Geo_Listboard(const std::string& prefix, unsigned short cols, Topl_Scene* scene) : Geo_Billboard(prefix, 1, cols, scene){}
};

struct Geo_Gridboard : Geo_Billboard {
    Geo_Gridboard(const std::string& prefix, unsigned short num) : Geo_Billboard(prefix, num, num){}
    Geo_Gridboard(const std::string& prefix,unsigned short num, Topl_Scene* scene) : Geo_Billboard(prefix, num, num, scene){}
};

#define GEO_LAYOUT_H
#endif