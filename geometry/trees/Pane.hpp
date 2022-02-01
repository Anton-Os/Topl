#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define PANE_RADIUS 0.5
#define PANE_BORDER 0.05f
// #define DEFAULT_PANE_Y 0.05
#define PANE_ROOT_Z 0.000002
#define PANE_CHILD_Z 0.000001

class Geo_Pane {
public:
    Geo_Pane(){}
    ~Geo_Pane(){
#ifdef RASTERON_H
        if(_background != nullptr) deleteImg(_background);
#endif
    }

    void setRenderObj(Geo_FlatSquare* square){ _square = square; }
    Geo_RenderObj* getRenderObj(){ return (Geo_RenderObj*)_square; }
	unsigned getColor() { return _bkColor; }
#ifdef RASTERON_H
	void setImageBk(Rasteron_Image* image) {
		if (_background != nullptr) deleteImg(_background); // previous image needs to be erased for override
		_background = image;
		if(std::string(image->name) == "blank")  _bkColor = *(image->data); // simplifies setting background color
	}
	Rasteron_Image* getBackground() { return _background; }
#endif
private:
	Geo_FlatSquare* _square = nullptr;
    unsigned _bkColor = 0xFFFFFFFF; // default background is white
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
		init(rows, columns);
		fill(scene);
	}

	Geo_PaneLayout(
		const std::string& prefix,
		Topl_Scene* scene,
		unsigned rows,
		unsigned columns,
		float radius,
		float border
	) : Geo_Tree(prefix, scene, &_dummyGeo, (rows * columns) + 1) {
		_radius = radius;
		_border = border;
		init(rows, columns);
		fill(scene);
	}

	Geo_Pane* getRootPane(){ return &_rootPane; }
	Geo_Pane* getChildPane(unsigned r, unsigned c);
private:
	void init(unsigned rows, unsigned columns); // creates panes and replaces all render objects
    void fill(Topl_Scene* scene) override;

	static Geo_FlatSquare _dummy;
	static Geo_Actor _dummyGeo;
    Geo_Pane _rootPane = Geo_Pane(); // root
	std::vector<Geo_Pane> _panes; // children
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(PANE_RADIUS, PANE_ROOT_Z);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(PANE_RADIUS, PANE_CHILD_Z);
    
	unsigned _rows; 
    unsigned _columns;
	float _radius = PANE_RADIUS;
	float _border = PANE_BORDER;
};