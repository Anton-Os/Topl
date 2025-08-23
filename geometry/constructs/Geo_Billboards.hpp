#ifndef GEO_LAYOUT_H

#include "meshes/Geo_Surface.hpp"

#include "Geo_Grid.hpp"

#define PANE_SIZE 0.5F
#define PANE_Z 0.0F

// #include <iostream>

class Geo_Billboard : public Geo_Grid {
public:
	Geo_Billboard(const std::string& prefix, unsigned short rows, unsigned short columns) : Geo_Grid(prefix, &childMesh, Geo_Grid_Params(
		std::make_pair(rows, PANE_SIZE / rows), 
		std::make_pair(columns, PANE_SIZE / columns), 
		std::make_pair(1, 0.0F))
	){ init(); }

	Geo_Billboard(const std::string& prefix, unsigned short rows, unsigned short columns, Topl_Scene* scene) : Geo_Grid(prefix, &childMesh, Geo_Grid_Params(
		std::make_pair(rows, PANE_SIZE / rows), 
		std::make_pair(columns, PANE_SIZE / columns), 
		std::make_pair(1, 0.0F))
	){
		init();
		configure(scene);
	}

    void init() override {
		_geoActors.resize(_params.getGridSize() + 1);
		if(rootBorder > 0.0F) rootMesh.scale({ 1.0F + rootBorder, 1.0F + rootBorder, 0.0F });
		_geoActors[_params.getGridSize()] = Geo_Actor(&rootMesh);
		childMesh.scale({ (1.0F / _params.x.first) - (rootBorder / 7.5F), (1.0F / _params.y.first) - (rootBorder / 7.5F), 0.0F });
#ifdef TOPL_ENABLE_TEXTURES
        rootImg.setImage(gradientImgOp(RASTERON_SIZE(SAMPLER_HEIGHT, SAMPLER_WIDTH), SIDE_Radial, 0xFF222222, 0xFF444444));
        rootImg.addBorder(0.05, 0xFF222222);
		for(unsigned p = 0; p < _params.getGridSize(); p++) paneSampler_map.insert({ &_geoActors.at(p), Sampler_2D(copyImgOp(rootImg.getImage())) });
#endif
	}

	void configure(Topl_Scene* scene) override {
		Geo_Grid::configure(scene);

		scene->addGeometry(getPrefix() + "root", &_geoActors.back()); 
#ifdef TOPL_ENABLE_TEXTURES
		scene->addTexture(getPrefix() + "root", &rootImg);
		for(unsigned p = 0; p < _params.getGridSize(); p++)
			scene->addTexture(getCellName(p + 1), &paneSampler_map.at(&_geoActors.at(p))); 
#endif
	}

	void scale(Vec3f scaleVec){
    	for(unsigned g = 0; g < _geoActors.size(); g++) {
			_geoActors[g].setSize(scaleVec);
			Vec3f offsetVec = *_geoActors[_params.getGridSize()].getPos() - *_geoActors[g].getPos();
			if(_params.getGridSize() != g)
				_geoActors[g].updatePos({ offsetVec[0] * (2.0F + (scaleVec[0] - 1.0F)), offsetVec[1] *  (2.0F + (scaleVec[1] - 1.0F)), 0.0F });
		}
		// TODO: Adjust images and textures
    }

    void expandHorz(std::pair<unsigned short, unsigned short> cell, unsigned short amount){
		unsigned index = (cell.second * _params.x.first) + cell.first;
		for(unsigned a = 0; a < _params.x.first * _params.y.first; a++) _geoActors[a].isShown = a != index - 1 && a != index + 1;
		_geoActors[index].updateSize({ (float)amount, 0.0F, 0.0F });
	}
	void expandVert(std::pair<unsigned short, unsigned short> cell, unsigned short amount){
		unsigned index = (cell.second * _params.x.first) + cell.first;
		for(unsigned a = 0; a < _params.x.first * _params.y.first; a++) _geoActors[a].isShown = a != index - _params.x.first && a != index + _params.x.first;
		_geoActors[index].updateSize({ 0.0F, (float)amount, 0.0F }); // TODO: Figure out exact proportions
	}
#ifdef TOPL_ENABLE_TEXTURES
	Sampler_2D* getImgRoot(){ return getImgAt(_params.getGridSize()); }
	Sampler_2D* getImgAt(unsigned short i){ return (i != _params.getGridSize())? &paneSampler_map.at(&_geoActors.at(i)) : &rootImg; }

    void resetState(){ // abstract the loop
		unsigned i = 0;

		for(auto p = paneItemUI_map.begin(); p != paneItemUI_map.end(); p++){
			if(p->second->getState() != MENU_None){
				p->second->setState(MENU_None);
				getImgAt(i)->setImage(p->second->stateImg.getImage());
			}
			i++;
		}
	}

	void setState(unsigned paneIndex, double x, double y){ // abstract the loop
		unsigned i = 0;

		for(auto p = paneItemUI_map.begin(); p != paneItemUI_map.end(); p++){
			if(paneIndex == i){
                if(p->second->getName().find("dial") != std::string::npos){ // checks for dial match
                    Sampler_Dial* dialUI = dynamic_cast<Sampler_Dial*>(&(*p->second));
                    if(dialUI != nullptr) dialUI->setState(x, y);
                }
                else if(p->second->getName().find("slider") != std::string::npos){ // checks for slider match
                    Sampler_Slider* sliderUI = dynamic_cast<Sampler_Slider*>(&(*p->second));
                    if(sliderUI != nullptr) sliderUI->setState(x);
                }
				getImgAt(i)->setImage(p->second->stateImg.getImage());
			}
			i++;
		}
    }

	void setState(unsigned paneIndex, bool isSelect){ // abstract the loop
		unsigned i = 0;

		for(auto p = paneItemUI_map.begin(); p != paneItemUI_map.end(); p++){
			if(paneIndex == i) p->second->setState((isSelect)? MENU_On : MENU_Pre); // This applies to buttons only
			else p->second->setState(MENU_None);

			getImgAt(i)->setImage(p->second->stateImg.getImage());
			i++;
		}
	}

	void overlay(unsigned paneIndex, Sampler_UI* element){ 
		paneItemUI_map.insert({ getGeoActor(paneIndex), element });
		getImgAt(paneIndex)->setImage(element->stateImg.getImage());
	}
#endif
protected:
	Geo_Quad2D childMesh = Geo_Quad2D(PANE_SIZE, PANE_Z + 0.000001F);
	// Geo_Actor childActor = Geo_Actor(&childMesh);
	float rootBorder = 0.05F;
	Geo_Quad2D rootMesh = Geo_Quad2D(PANE_SIZE, PANE_Z);
	Geo_Actor rootActor = Geo_Actor(&rootMesh);
#ifdef TOPL_ENABLE_TEXTURES
	Sampler_2D rootImg; // root background
	std::map<const Geo_Actor*, Sampler_2D> paneSampler_map; // for child images
	std::map<const Geo_Actor*, Sampler_UI*> paneItemUI_map; // for child UI elements;
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
