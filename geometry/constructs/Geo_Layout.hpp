
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
		std::replace(fontFilePath.begin(), fontFilePath.end(), '/', '\\');

		Rasteron_Text textObj = { fontFilePath.c_str(), "X", 0xFFEEEEEE, 0xFF000000 };
		rootImg.setColorImage(0xFF8822EE); // rootImg.setTextImage(&textObj);
		scene->addTexture(getPrefix() + "root", &rootImg);
		for(unsigned p = 0; p < _params.getGridSize(); p++){
			std::string paneName = std::to_string(p + 1);
			textObj.text = paneName.c_str();
			paneImgs.insert({ &_geoActors.at(p), Img_Base() });
			paneImgs.at(&_geoActors.at(p)).setTextImage(&textObj);
			{
				Rasteron_Image* testImage = solidImgOp({ 1024, 1024 }, getPaneColor(p));
				for(unsigned p = 0; p < 1024 * 1024; p++){
					double x = (1.0 / (double)1024) * (unsigned)(p % 1024);
		    		double y = (1.0 / (double)1024) * (double)(p / 1024);

					if(sqrt((fabs(0.5 - x) * fabs(0.5 - x)) + (fabs(0.5 - y) * fabs(0.5 - y))) < 0.35) 
						*(testImage->data + p) = 0xFF8822EE; // 0xFFFFFFFF - getPaneColor(p);
				}
				paneImgs.at(&_geoActors.at(p)).setImage(testImage);
				dealloc_image(testImage);
			}
			scene->addTexture(getCellName(p + 1), &paneImgs.at(&_geoActors.at(p)));

			paneImgArrays.insert({ &_geoActors.at(p), Img_Array() });
			for(unsigned t = 1; t < MAX_TEX_BINDINGS; t++){
				Rasteron_Image* stageImg = copyImgOp(paneImgs.at(&_geoActors.at(p)).getImage());
				for(unsigned i = 0; i < stageImg->width * stageImg->height; i++)
					if(*(stageImg->data + i) == 0xFFEEEEEE) *(stageImg->data + i) = getPaneColor(t);
				addFrameAt(paneImgArrays.at(&_geoActors.at(p)).getQueue(), stageImg, t);
				dealloc_image(stageImg);
			}
			// scene->addArrayTex(getCellName(p + 1), &paneImgArrays.at(&_geoActors.at(p))); // Uncomment this
		}
#endif
	}
protected:
	Geo_Quad2D childMesh = Geo_Quad2D(PANE_SIZE, PANE_Z + 0.0001F);
	// Geo_Actor childActor = Geo_Actor(&childMesh);
	float rootBorder = 0.05F;
	Geo_Quad2D rootMesh = Geo_Quad2D(PANE_SIZE, PANE_Z);
	Geo_Actor rootActor = Geo_Actor(&rootMesh);

#ifdef RASTERON_H
	unsigned getPaneColor(unsigned short index){
		switch(index){
			case 0: return 0xFFEEEEEE;
			case 1: return 0xFF0000FF; case 2: return 0xFF00FF00; case 3: return 0xFFFF0000;
			case 4: return 0xFFFFFF00; case 5: return 0xFFFF00FF; case 6: return 0xFF00FFFF;
			case 7: return 0xFF333333;
			default: return 0xFF000000;
		}
	}

	Img_Base rootImg; // root background
	std::map<const Geo_Actor*, Img_Base> paneImgs; // child backgrounds
	std::map<const Geo_Actor*, Img_Array> paneImgArrays; // child backgrounds in array
	// std::vector<Rasteron_Queue*> childStateBg;

	std::vector<pickerCallback> _pickerFuncs;
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