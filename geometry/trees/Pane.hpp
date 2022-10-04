#include "ShapesGen.hpp" // #include "Geometry.hpp"
#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define PANE_RADIUS 0.5
#define PANE_BORDER 0.05f
#define PANE_IMAGE_COLOR 0xAAFFFFFF // semi-transpertent default color
#define PANE_IMAGE_WIDTH 256
#define PANE_IMAGE_HEIGHT 256
#define PANE_Z 0.0f

typedef void (*paneCallback)(void);

class Geo_Pane {
public:
	Geo_Pane(unsigned color){ // Fixed 	Color Constructor
		_paneColor = color;
#ifdef RASTERON_H
		_presetImage = createSolidImg({PANE_IMAGE_HEIGHT, PANE_IMAGE_WIDTH }, _paneColor);
#endif
	}
    Geo_Pane(){ // Random Color Constructor
		_paneColor = genRandColor();
#ifdef RASTERON_H
		_presetImage = createSolidImg({ PANE_IMAGE_HEIGHT, PANE_IMAGE_WIDTH }, _paneColor);
#endif
	}
    ~Geo_Pane(){
#ifdef RASTERON_H
        if(_presetImage != nullptr) deleteImg(_presetImage);
		if (_selectImage != nullptr) deleteImg(_selectImage);
#endif
    }

	void addCallback(paneCallback call){ call = callback; }
	unsigned getColor() { return _paneColor; }
#ifdef RASTERON_H
	void selectImage(Rasteron_Image* image) { _selectImage = image; }
	const Rasteron_Image* getBackground() {
		return (_selectImage == nullptr)? _presetImage : _selectImage; 
	}
#endif
	paneCallback callback = nullptr;
private:
    unsigned _paneColor = PANE_IMAGE_COLOR;
#ifdef RASTERON_H
    Rasteron_Image* _presetImage = nullptr; // internal
	Rasteron_Image* _selectImage = nullptr; // external
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
		_width = radius; _height = radius;
		_border = border;
		resize(rows, columns);
	}

	Geo_PaneLayout( // Separate radii
		const std::string& prefix,
		unsigned rows, unsigned columns,
		float width, float height,
		float border
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		_width = width; _height = height;
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
	Geo_FlatSquare _rootSquare = Geo_FlatSquare(PANE_RADIUS, PANE_Z);
	Geo_FlatSquare _childSquare = Geo_FlatSquare(PANE_RADIUS, PANE_Z);
    
	unsigned _rows, _columns; 
	float _width = PANE_RADIUS; 
	float _height = PANE_RADIUS;
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
		float width, float height,
		float border
	) : Geo_PaneLayout(prefix, 1, 1, width, height, border) 
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
		float width, float height,
		float border
	) : Geo_PaneLayout(prefix, rows, 1, width, height, border)
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
		float width, float height,
		float border
	) : Geo_PaneLayout(prefix, 1, columns, width, height, border)
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
		float width, float height,
		float border
	) : Geo_PaneLayout(prefix, count, count, width, height, border) 
	{  }
};