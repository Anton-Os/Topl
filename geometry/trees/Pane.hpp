#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

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
        _square->modify(stretchTform, 1.0 / rows, AXIS_Y);
        _square->modify(stretchTform, 1.0 / columns, AXIS_X);
        // _actor->setRenderObj((Geo_RenderObj*)&_square);
    }

    // void setActor(Geo_Actor* actor){ _actor = actor; }
	void setRenderObj(Geo_FlatSquare* square) { _square = square; }
	void createSolidBk(unsigned color) {
		_bkColor = color;
#ifdef RASTERON_H
		// _background = createImgBlank(256, 256, _bkColor);
#endif
	}
#ifdef RASTERON_H
    Rasteron_Image* getBackground(){ return _background; }
#endif
private:
	Geo_FlatSquare* _square = nullptr;
    // Geo_Actor* _actor = nullptr;
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
	) : Geo_Tree(prefix, scene, &_squareGeo, (rows * columns) + 1) {
		_panes.resize(getActorCount() - 1);
		_paneSquares = (Geo_FlatSquare**)malloc(((rows * columns) + 1) * sizeof(Geo_FlatSquare*));
		for(unsigned p = 0; p < (rows * columns) + 1; p++){
			*(_paneSquares + p) = (Geo_FlatSquare*)malloc(sizeof(Geo_FlatSquare));
			*(*(_paneSquares + p)) = _square;

		}

        _rows = rows;
        _columns = columns;
        fill(scene);
    }

	~Geo_PaneLayout() {
		for (unsigned p = 0; p < (_rows * _columns) + 1; p++) free(*(_paneSquares + p));
		free(_paneSquares);
	}
private:
    void fill(Topl_Scene* scene) override;

	static Geo_FlatSquare _square;
	static Geo_Actor _squareGeo;
	Geo_FlatSquare** _paneSquares;
    Geo_Pane _rootPane = Geo_Pane(); // all panes are children positioned relative to the root pane
	std::vector<Geo_Pane> _panes;
    
    unsigned _rows; 
    unsigned _columns;
};