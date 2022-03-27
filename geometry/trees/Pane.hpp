#include "ShapesGen.hpp" // #include "Geometry.hpp"
#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define PANE_RADIUS 0.5
#define PANE_BORDER 0.05f
#define PANE_IMAGE_COLOR 0xFFFFFFFF // default is white
#define PANE_IMAGE_WIDTH 256
#define PANE_IMAGE_HEIGHT 256
#define PANE_ROOT_Z 0.000002
#define PANE_CHILD_Z 0.000001

typedef void (*paneCallback)(void);

class Geo_Pane {
public:
	Geo_Pane(unsigned color){ // Fixed 	Color Constructor
		_paneColor = color;
#ifdef RASTERON_H
		_internalImage = createImgBlank(PANE_IMAGE_HEIGHT, PANE_IMAGE_WIDTH, _paneColor);
#endif
	}
    Geo_Pane(){ // Random Color Constructor
		_paneColor = genRandColorVal();
#ifdef RASTERON_H
		_internalImage = createImgBlank(PANE_IMAGE_HEIGHT, PANE_IMAGE_WIDTH, _paneColor);
#endif
	}
    ~Geo_Pane(){
#ifdef RASTERON_H
        if(_internalImage != nullptr) deleteImg(_internalImage);
#endif
    }

	void addCallback(paneCallback call){ call = callback; }
	unsigned getColor() { return _paneColor; }
#ifdef RASTERON_H
	void selectImage(const Rasteron_Image* image) { _selectImage = image; }
	const Rasteron_Image* getBackground() {
		return (_selectImage == nullptr)? _internalImage : _selectImage; 
	}
#endif
	paneCallback callback = nullptr;
private:
    unsigned _paneColor = PANE_IMAGE_COLOR;
#ifdef RASTERON_H
    Rasteron_Image* _internalImage = nullptr; // default internal background
	const Rasteron_Image* _selectImage = nullptr; // selected external backgroundd
#endif
};

class Geo_PaneLayout : public Geo_Tree {
public:
	Geo_PaneLayout(
		const std::string& prefix,
		unsigned rows,
		unsigned columns
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		resize(rows, columns);
	}

	Geo_PaneLayout(
		const std::string& prefix,
		unsigned rows,
		unsigned columns,
		float radius,
		float border
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		_radius = radius;
		_border = border;
		resize(rows, columns);
	}

	Geo_Pane* getRootPane(){ return &_rootPane; }
	Geo_Pane* getChildPane(unsigned index);
	unsigned getRowCount(){ return _rows; }
	unsigned getColCount(){ return _columns; }

	void configure(Topl_Scene* scene) override;
	bool interact(float xPos, float yPos, unsigned color); // returns true if inside pane bounds, can fire callback based on color
protected:
	void resize(unsigned rows, unsigned columns); // creates panes and replaces all render objects

    Geo_Pane _rootPane = Geo_Pane(PANE_IMAGE_COLOR); // root
	std::vector<Geo_Pane> _panes; // children
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(PANE_RADIUS, PANE_ROOT_Z);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(PANE_RADIUS, PANE_CHILD_Z);
    
	unsigned _rows; 
    unsigned _columns;
	float _radius = PANE_RADIUS;
	float _border = PANE_BORDER;

	static Geo_FlatSquare _decoySquare;
	static Geo_Actor _decoyActor;
};

// UnitLayout has only one child pane
class Geo_UnitLayout : public Geo_PaneLayout {
public:
	Geo_UnitLayout(
		const std::string& prefix
	) : Geo_PaneLayout(prefix, 1, 1) 
	{  }

	Geo_UnitLayout(
		const std::string& prefix,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, 1, 1, radius, border) 
	{  }
};

// RowLayout has only rows
class Geo_RowLayout : public Geo_PaneLayout {
public:
	Geo_RowLayout(
		const std::string& prefix,
		unsigned rows
	) : Geo_PaneLayout(prefix, rows, 1) 
	{  }

	Geo_RowLayout(
		const std::string& prefix,
		unsigned rows,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, rows, 1, radius, border) 
	{  }
};

// BoxedLayout has matching row and column count
class Geo_BoxedLayout : public Geo_PaneLayout {
public:
	Geo_BoxedLayout(
		const std::string& prefix,
		unsigned count
	) : Geo_PaneLayout(prefix, count, count) 
	{  }

	Geo_BoxedLayout(
		const std::string& prefix,
		unsigned count,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, count, count, radius, border) 
	{  }
};