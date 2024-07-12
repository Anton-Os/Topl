
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

	void init(){
		_geoActors.resize(_params.getGridSize() + 1);
		if(rootBorder > 0.0F) rootMesh.scale({ 1.0F + rootBorder, 1.0F + rootBorder, 0.0F });
		_geoActors[_params.getGridSize()] = Geo_Actor(&rootMesh);
		childMesh.scale({ (1.0F / _params.x.first) - (rootBorder / 7.5F), (1.0F / _params.y.first) - (rootBorder / 7.5F), 0.0F });
		for(unsigned p = 0; p < _params.getGridSize(); p++) paneImg_map.insert({ &_geoActors.at(p), Img_Base(0xFF666666) });
	}

	void configure(Topl_Scene* scene) override {
		Geo_Grid::configure(scene);

		scene->addGeometry(getPrefix() + "root", &_geoActors.back()); 
#ifdef RASTERON_H
		scene->addTexture(getPrefix() + "root", &rootImg);
		for(unsigned p = 0; p < _params.getGridSize(); p++)
			scene->addTexture(getCellName(p + 1), &paneImg_map.at(&_geoActors.at(p))); 
#endif
	}
	void scale(Vec3f scaleVec){
    	for(unsigned g = 0; g < _geoActors.size(); g++) {
			_geoActors[g].setSize(scaleVec);
			Vec3f offsetVec = *_geoActors[_params.getGridSize()].getPos() - *_geoActors[g].getPos();
			if(_params.getGridSize() != g)
				_geoActors[g].updatePos({ offsetVec[0] * (2.0F + (scaleVec[0] - 1.0F)), offsetVec[1] *  (2.0F + (scaleVec[1] - 1.0F)), 0.0F });
		}
    }

	Img_Base* getImgAt(unsigned short i){ return (i != _params.getGridSize())? &paneImg_map.at(&_geoActors.at(i)) : &rootImg; }

	void resetState(){
		/* for(unsigned p = 0; p < _params.getGridSize(); p++){
			const Geo_Actor* targetActor = getGeoActor(p);
			auto paneItemUI = std::find_if(paneItemUI_map.begin(), paneItemUI_map.end(), [targetActor](const std::pair<const Geo_Actor*, Img_UI*>& i){ return i.first == targetActor; });
			if(paneItemUI != paneItemUI_map.end()){
				paneItemUI->second->setState(MENU_None);
				getImgAt(p)->setImage(paneItemUI->second->stateImg.getImage());
			}
		} */
	}

	void setState(unsigned paneIndex, bool isSelect, double x, double y){
		if(paneIndex >= _params.getGridSize()) std::cout << "Grid arg out of range" << std::endl;
		else for(unsigned p = 0; p < _params.getGridSize(); p++){
			const Geo_Actor* targetActor = getGeoActor(p);
			auto paneItemUI = std::find_if(paneItemUI_map.begin(), paneItemUI_map.end(), [targetActor](const std::pair<const Geo_Actor*, Img_UI*>& i){ return i.first == targetActor; });
			if(paneItemUI != paneItemUI_map.end()){
				std::cout << "Setting state for index" << std::to_string(paneIndex) << " with name " << paneItemUI->second->getName() << std::endl;

				if((paneItemUI->second->getName().find("button") != std::string::npos) && paneItemUI->second->getState() != MENU_Off)
					(p == paneIndex) ? paneItemUI->second->setState((isSelect)? MENU_On : MENU_Pre) : paneItemUI->second->setState(MENU_None);
				else if((paneItemUI->second->getName().find("check") != std::string::npos || paneItemUI->second->getName().find("label") != std::string::npos) && p == paneIndex && paneItemUI->second->getState() != MENU_Off)
					paneItemUI->second->setState(MENU_On); // for testing
				// 	(paneItemUI->second->getState() == MENU_None)? paneItemUI->second->setState((isSelect)? MENU_On : MENU_Pre) : paneItemUI->second->setState(MENU_None);
				else if(paneItemUI->second->getName().find("dial") != std::string::npos && p == paneIndex){
					Img_Dial* dialUI = dynamic_cast<Img_Dial*>(&(*paneItemUI->second));
					(dialUI != nullptr)? dialUI->setState(x, y) : std::cout << "Null pointer cast!" << std::endl;
				}
				else if(paneItemUI->second->getName().find("slider") != std::string::npos && p == paneIndex){
					Img_Slider* sliderUI = dynamic_cast<Img_Slider*>(&(*paneItemUI->second));
					(sliderUI != nullptr)? sliderUI->setState(x) : std::cout << "Null pointer cast!" << std::endl;
				}
				// else std::cerr << "State setting unsupported for name" << paneItemUI->second->getName() << std::endl;
				getImgAt(paneIndex)->setImage(paneItemUI->second->stateImg.getImage()); // syncronize state with pane image
			} else std::cerr << "State setting not found" << std::endl;
		}
	}

	void setState(unsigned paneIndex, bool isSelect){ setState(paneIndex, isSelect, 0.5, 0.5); }

	void overlay(unsigned paneIndex, Img_UI* element){ 
		paneItemUI_map.insert({ getGeoActor(paneIndex), element });
		getImgAt(paneIndex)->setImage(element->stateImg.getImage());
	}
protected:
	Geo_Quad2D childMesh = Geo_Quad2D(PANE_SIZE, PANE_Z + 0.0001F);
	// Geo_Actor childActor = Geo_Actor(&childMesh);
	float rootBorder = 0.05F;
	Geo_Quad2D rootMesh = Geo_Quad2D(PANE_SIZE, PANE_Z);
	Geo_Actor rootActor = Geo_Actor(&rootMesh);
#ifdef RASTERON_H
	Img_Base rootImg = Img_Base(0xFF333333); // root background
	std::map<const Geo_Actor*, Img_Base> paneImg_map; // for child images
	std::map<const Geo_Actor*, Img_UI*> paneItemUI_map; // for child UI elements;
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