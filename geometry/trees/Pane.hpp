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
		unsigned rows, unsigned columns
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		resize(rows, columns);
	}

	Geo_PaneLayout( // Matching radii
		const std::string& prefix,
		unsigned rows, unsigned columns,
		float radius,
		float border
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		_xRadius = radius; _yRadius = radius;
		_border = border;
		resize(rows, columns);
	}

	Geo_PaneLayout( // Separate radii
		const std::string& prefix,
		unsigned rows, unsigned columns,
		float xRadius, float yRadius,
		float border
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		_xRadius = xRadius; _yRadius = yRadius;
		_border = border;
		resize(rows, columns);
	}

	Geo_Pane* getRootPane(){ return &_rootPane; }
	Geo_Pane* getChildPane(unsigned index);
	unsigned getRowCount(){ return _rows; }
	unsigned getColCount(){ return _columns; }

	void configure(Topl_Scene* scene) override;
	bool interact(float xPos, float yPos, unsigned color); // returns true within pane bounds, callback based on color
protected:
	void resize(unsigned rows, unsigned columns); // creates panes and replaces all render objects

    Geo_Pane _rootPane = Geo_Pane(PANE_IMAGE_COLOR); // root
	std::vector<Geo_Pane> _panes; // children
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(PANE_RADIUS, PANE_ROOT_Z);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(PANE_RADIUS, PANE_CHILD_Z);
    
	unsigned _rows, _columns; 
	float _xRadius = PANE_RADIUS; // length along X axis
	float _yRadius = PANE_RADIUS; // length along Y axis
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

	Geo_UnitLayout( // Matching radii
		const std::string& prefix,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, 1, 1, radius, border) 
	{  }

	Geo_UnitLayout( // Separate radii
		const std::string& prefix,
		float xRadius, float yRadius,
		float border
	) : Geo_PaneLayout(prefix, 1, 1, xRadius, yRadius, border) 
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

	Geo_RowLayout( // Matching radii
		const std::string& prefix,
		unsigned rows,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, rows, 1, radius, border) 
	{  }

	Geo_RowLayout( // Separate radii
		const std::string& prefix,
		unsigned rows,
		float xRadius, float yRadius,
		float border
	) : Geo_PaneLayout(prefix, rows, 1, xRadius, yRadius, border)
	{  }
};

// ColLayout has only columns
class Geo_ColLayout : public Geo_PaneLayout {
public:
	Geo_ColLayout(
		const std::string& prefix,
		unsigned columns
	) : Geo_PaneLayout(prefix, 1, columns) 
	{  }

	Geo_ColLayout( // Matching radii
		const std::string& prefix,
		unsigned columns,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, 1, columns, radius, border) 
	{  }

	Geo_ColLayout( // Separate radii
		const std::string& prefix,
		unsigned columns,
		float xRadius, float yRadius,
		float border
	) : Geo_PaneLayout(prefix, 1, columns, xRadius, yRadius, border)
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

	Geo_BoxedLayout( // Matching radii
		const std::string& prefix,
		unsigned count,
		float radius,
		float border
	) : Geo_PaneLayout(prefix, count, count, radius, border) 
	{  }

	Geo_BoxedLayout( // Separate radii
		const std::string& prefix,
		unsigned count,
		float xRadius, float yRadius,
		float border
	) : Geo_PaneLayout(prefix, count, count, xRadius, yRadius, border) 
	{  }
};