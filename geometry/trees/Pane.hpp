#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define PANE_RADIUS 0.5
#define PANE_BORDER 0.01
// #define DEFAULT_PANE_Y 0.05
#define PANE_Z 0.05

static float stretchTform(float input, double mod){ return input * mod; }

class Geo_Pane {
public:
    Geo_Pane(){}
    ~Geo_Pane(){
#ifdef RASTERON_H
        if(_background != nullptr) deleteImg(_background);
#endif
    }


	void createSolidBk(unsigned color) {
		_bkColor = color;
#ifdef RASTERON_H
		_background = createImgBlank(256, 256, _bkColor);
#endif
	}
	void createImageBk(Rasteron_Image* image){
#ifdef RASTERON_H
		_background = image;
#endif
	}
#ifdef RASTERON_H
    Rasteron_Image* getBackground(){ return _background; }
#endif
    void setRenderObj(Geo_FlatSquare* square){ _square = square; }
    Geo_RenderObj* getRenderObj(){ return (Geo_RenderObj*)_square; }
private:
	Geo_FlatSquare* _square = nullptr;
    unsigned _bkColor = 0xFF000000; // starting background is black
#ifdef RASTERON_H
    Rasteron_Image* _background = nullptr; // background image available if Rasteron is supported
#endif
};

class Geo_PaneLayout : public Geo_Tree {
public:
	Geo_PaneLayout(
		const std::string& prefix,
		Topl_Scene* scene,
		unsigned rows,
		unsigned columns
	) : Geo_Tree(prefix, scene, &_dummyGeo, (rows * columns) + 1) {
		// _rootPane.setRenderObj(&_rootSquare);
		_rootPane.createSolidBk(0xFF0000FF); // blue backgroud

		_panes.resize(getActorCount() - 1);
		_childSquare.modify(stretchTform, (1.0 / rows) - PANE_BORDER, AXIS_Y);
		_childSquare.modify(stretchTform, (1.0 / columns) - PANE_BORDER, AXIS_X);
		for (std::vector<Geo_Pane>::iterator currentPane = _panes.begin(); currentPane < _panes.end(); currentPane++)
			currentPane->createSolidBk(0xFFFF0000); // red background
			// currentPane->setRenderObj(&_childSquare);

		_rows = rows;
		_columns = columns;
		fill(scene);
	}
private:
    void fill(Topl_Scene* scene) override;

	static Geo_FlatSquare _dummy;
	static Geo_Actor _dummyGeo;
    Geo_Pane _rootPane = Geo_Pane(); // root
	std::vector<Geo_Pane> _panes; // children
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(PANE_RADIUS);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(PANE_RADIUS);
    
    unsigned _rows; 
    unsigned _columns;
};