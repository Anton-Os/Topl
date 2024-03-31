
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

	~Geo_Billboard(){
#ifdef RASTERON_H
	/* dealloc_image(rootBk);
	for(std::vector<Rasteron_Queue*>::iterator q = childStateBg.begin(); q != childStateBg.end(); q++)
		free_queue(*q); */
#endif
	}

	void configure(Topl_Scene* scene) override {
		_geoActors.resize(_params.getGridSize() + 1);
		if(rootBorder > 0.0F) rootMesh.scale({ 1.0F + rootBorder, 1.0F + rootBorder, 0.0F });
		_geoActors[_params.getGridSize()] = Geo_Actor(&rootMesh);

		// childMesh.scale({ 1.25F, 1.25F, 1.25F });
		childMesh.scale({ 1.0F / _params.x.first, 1.0F / _params.y.first, 0.0F });
		Geo_Grid::configure(scene);

		scene->addGeometry(getPrefix() + "root", &_geoActors.back());
#ifdef RASTERON_H
		std::string fontFilePath = std::string(FONTS_DIR) + "Tw-Cen-MT.ttf";
		Rasteron_Text textObj = { fontFilePath.c_str(), "X", 0xFFEEEEEE, 0xFF000000 };

		rootImg.setColorImage(0xFF333333);
		scene->addTexture(getPrefix() + "root", &rootImg);
		for(unsigned p = 0; p < _params.getGridSize(); p++){
			paneImg_map.insert({ &_geoActors.at(p), Img_Base() });
			// paneImg_map.at(&_geoActors.at(p)).setTextImage(&textObj);
			paneImg_map.at(&_geoActors.at(p)).setColorImage(0xFFEEEEEE);

			paneImgArray_map.insert({ &_geoActors.at(p), Img_Array() });
			for(unsigned t = 1; t < MAX_TEX_BINDINGS; t++){
				textObj.fgColor = RAND_COLOR(); textObj.bkColor = RAND_COLOR();
				Img_Base image = Img_Base(textObj);
				addFrameAt(paneImgArray_map.at(&_geoActors.at(p)).getQueue(), image.getImage(), t);
			}
			scene->addArrayTex(getCellName(p + 1), &paneImgArray_map.at(&_geoActors.at(p))); 
		}
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
protected:
	Geo_Quad2D childMesh = Geo_Quad2D(PANE_SIZE, PANE_Z + 0.0001F);
	// Geo_Actor childActor = Geo_Actor(&childMesh);
	float rootBorder = 0.05F;
	Geo_Quad2D rootMesh = Geo_Quad2D(PANE_SIZE, PANE_Z);
	Geo_Actor rootActor = Geo_Actor(&rootMesh);

#ifdef RASTERON_H
	Img_Base rootImg; // root background
	std::map<const Geo_Actor*, Img_Base> paneImg_map; // child backgrounds
	std::map<const Geo_Actor*, Img_Array> paneImgArray_map; // child backgrounds in array
	// std::vector<Rasteron_Queue*> childStateBg;
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