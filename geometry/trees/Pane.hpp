#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define DEFAULT_PANE_SIZE 0.1
#define DEFAULT_Y_INC 0.05
#define DEFAULT_Z_INC 0.01

static float stretchTform(float input, double mod){ return input * mod; }

class Geo_Pane {
public:
    Geo_Pane(){
		createSolidBk(0xFF000000); // black background
    }
    Geo_Pane(unsigned color){
		createSolidBk(color); // custom color background
    }
#ifdef RASTERON_H
	Geo_Pane(std::string filePath) {
		// create image from filePath, set as background
	}
	Geo_Pane(std::string text, std::string fontFilePath, unsigned fgColor, unsigned bkColor) {
		// render text using font and fontFilePath
	}
#endif
    ~Geo_Pane(){
#ifdef RASTERON_H
        if(_background != nullptr) deleteImg(_background);
#endif
    }

    void scalePane(unsigned rows, unsigned columns){
		if (_square == nullptr) return; // object cannot be modified if null[tr
        // _square->modify(stretchTform, 1.0 / rows, AXIS_Y);
        // _square->modify(stretchTform, 1.0 / columns, AXIS_X);
    }

	void createSolidBk(unsigned color) {
		_bkColor = color;
#ifdef RASTERON_H
		// _background = createImgBlank(256, 256, _bkColor);
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
		_panes.resize(getActorCount() - 1);
		_childSquare.modify(stretchTform, 1.0 / rows, AXIS_Y);
		_childSquare.modify(stretchTform, 1.0 / columns, AXIS_X);
		for (std::vector<Geo_Pane>::iterator currentPane = _panes.begin(); currentPane < _panes.end(); currentPane++)
			currentPane->setRenderObj(&_childSquare);

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
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(DEFAULT_PANE_SIZE);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(DEFAULT_PANE_SIZE * 0.5);
    
    unsigned _rows; 
    unsigned _columns;
};