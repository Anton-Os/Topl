#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

// Pane Object

#define PANE_IMAGE_WIDTH 256
#define PANE_IMAGE_HEIGHT 256
#define PANE_Z 0.0f

class Geo_Pane {
public:
	Geo_Pane() : _actor(nullptr) {
#ifdef RASTERON_H
		_backgroundImg.setColorImage(genRandColor()); // random color assignment
#endif
	}

	Geo_Pane(Geo_Actor* a) : _actor(a) {
#ifdef RASTERON_H
		_backgroundImg.setColorImage(genRandColor()); // random color assignment
#endif
	}

	Geo_Actor* getActor() { return _actor; }
	Img_Base* getBackground() { return &_backgroundImg; }
private:
	Geo_Actor* _actor;
	Img_Base _backgroundImg;
};

// Pane Layouts

#define PANE_RADIUS 0.5
#define PANE_BORDER 0.05f

class Geo_PaneLayout : public Geo_Tree {
public:
	Geo_PaneLayout(
		const std::string& prefix,
		unsigned rows, unsigned columns
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		resize(rows, columns);
	}

	Geo_PaneLayout( // radius param
		const std::string& prefix,
		unsigned rows, unsigned columns,
		float radius, float border
	) : Geo_Tree(prefix, &_decoyActor, (rows * columns) + 1) {
		_width = radius; _height = radius;
		_border = border;
		resize(rows, columns);
	}

	Geo_PaneLayout( // width and height params
		const std::string& prefix,
		unsigned rows, unsigned columns,
		float width, float height, float border
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
protected:
	void resize(unsigned rows, unsigned columns); // creates panes and replaces all render objects

    Geo_Pane _rootPane = Geo_Pane(); // root
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

// Extended Layouts

class Geo_UnitLayout : public Geo_PaneLayout { // UnitLayout with only one child
public:
	Geo_UnitLayout( // default
		const std::string& prefix
	) : Geo_PaneLayout(prefix, 1, 1){}

	Geo_UnitLayout( // radius param
		const std::string& prefix,
		float radius, float border
	) : Geo_PaneLayout(prefix, 1, 1, radius, border){}

	Geo_UnitLayout( // width and height params
		const std::string& prefix,
		float width, float height, float border
	) : Geo_PaneLayout(prefix, 1, 1, width, height, border){}
};

class Geo_RowLayout : public Geo_PaneLayout { // RowLayout with only rows
public:
	Geo_RowLayout( // default
		const std::string& prefix,
		unsigned rows
	) : Geo_PaneLayout(prefix, rows, 1){}

	Geo_RowLayout( // radius param
		const std::string& prefix,
		unsigned rows, 
		float radius, float border
	) : Geo_PaneLayout(prefix, rows, 1, radius, border){}

	Geo_RowLayout( // width and height params
		const std::string& prefix,
		unsigned rows,
		float width, float height, float border
	) : Geo_PaneLayout(prefix, rows, 1, width, height, border){}
};

class Geo_ColLayout : public Geo_PaneLayout { // ColLayout with only columns
public:
	Geo_ColLayout( // default
		const std::string& prefix,
		unsigned columns
	) : Geo_PaneLayout(prefix, 1, columns){}

	Geo_ColLayout( // radius param
		const std::string& prefix,
		unsigned columns,
		float radius, float border
	) : Geo_PaneLayout(prefix, 1, columns, radius, border){}

	Geo_ColLayout( // width and height params
		const std::string& prefix,
		unsigned columns,
		float width, float height, float border
	) : Geo_PaneLayout(prefix, 1, columns, width, height, border){}
};

class Geo_BoxedLayout : public Geo_PaneLayout { // BoxedLayout with matching row and column count
public:
	Geo_BoxedLayout( // default
		const std::string& prefix,
		unsigned count
	) : Geo_PaneLayout(prefix, count, count) {}

	Geo_BoxedLayout( // radius param
		const std::string& prefix,
		unsigned count,
		float radius, float border
	) : Geo_PaneLayout(prefix, count, count, radius, border) {}

	Geo_BoxedLayout( // width and height params
		const std::string& prefix,
		unsigned count,
		float width, float height, float border
	) : Geo_PaneLayout(prefix, count, count, width, height, border) {}
};