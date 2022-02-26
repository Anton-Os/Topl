#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define PANE_RADIUS 0.5
#define PANE_BORDER 0.05f
#define PANE_BK_COLOR 0xFFFFFFFF // default is white
#define PANE_BK_WIDTH 256
#define PANE_BK_HEIGHT 256
#define PANE_ROOT_Z 0.000002
#define PANE_CHILD_Z 0.000001

class Geo_Pane {
public:
	Geo_Pane(unsigned color){ // Fixed 	Color Constructor
		_bkColor = color;
#ifdef RASTERON_H
		_bkInternal = createImgBlank(PANE_BK_HEIGHT, PANE_BK_WIDTH, _bkColor);
#endif
	}
    Geo_Pane(){ // Random Color Constructor
		_bkColor = genRandColorVal();
#ifdef RASTERON_H
		_bkInternal = createImgBlank(PANE_BK_HEIGHT, PANE_BK_WIDTH, _bkColor);
#endif
	}
    ~Geo_Pane(){
#ifdef RASTERON_H
        if(_bkInternal != nullptr) deleteImg(_bkInternal);
#endif
    }

	unsigned getColor() { return _bkColor; }
#ifdef RASTERON_H
	void selectBk(const Rasteron_Image* image) { _bkSelection = image; }
	const Rasteron_Image* getBackground() {
		return (_bkSelection == nullptr)? _bkInternal : _bkSelection; 
	}
#endif
private:
    unsigned _bkColor = PANE_BK_COLOR;
#ifdef RASTERON_H
    Rasteron_Image* _bkInternal = nullptr; // default background managed internally
	const Rasteron_Image* _bkSelection = nullptr; // selected background managed externally
#endif
};

class Geo_PaneLayout : public Geo_Tree {
public:
	Geo_PaneLayout(
		const std::string& prefix,
		// Topl_Scene* scene,
		unsigned rows,
		unsigned columns
	) : Geo_Tree(prefix, &_dummyGeo, (rows * columns) + 1) {
		resize(rows, columns);
		// init(scene);
	}

	Geo_PaneLayout(
		const std::string& prefix,
		// Topl_Scene* scene,
		unsigned rows,
		unsigned columns,
		float radius,
		float border
	) : Geo_Tree(prefix, &_dummyGeo, (rows * columns) + 1) {
		_radius = radius;
		_border = border;
		resize(rows, columns);
		// init(scene);
	}

	Geo_Pane* getRootPane(){ return &_rootPane; }
	Geo_Pane* getChildPane(unsigned index);
	unsigned getRowCount(){ return _rows; }
	unsigned getColCount(){ return _columns; }

	void init(Topl_Scene* scene) override;
private:
	void resize(unsigned rows, unsigned columns); // creates panes and replaces all render objects

    Geo_Pane _rootPane = Geo_Pane(PANE_BK_COLOR); // root
	std::vector<Geo_Pane> _panes; // children
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(PANE_RADIUS, PANE_ROOT_Z);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(PANE_RADIUS, PANE_CHILD_Z);
    
	unsigned _rows; 
    unsigned _columns;
	float _radius = PANE_RADIUS;
	float _border = PANE_BORDER;

	static Geo_FlatSquare _dummy;
	static Geo_Actor _dummyGeo;
};