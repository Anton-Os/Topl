#include "primitives/Geo_Flat.hpp"

#include "Geo_Tree.hpp"

#define DEFAULT_Y_INC 0.05
#define DEFAULT_Z_INC 0.01

static float scale(float input, double mod){
    return input * mod;
}

class Geo_Pane {
public:
    Geo_Pane(Geo_Actor* a){ 
        _actor = a;
        _bkColor = (a->getId() % 2)? 0xFFFF0000 : 0xFF0000FF; // switch to red or blue background
    }

    void scalePane(unsigned rows, unsigned columns){
        // _square.modify(&scale, 1.0 / rows, AXIS_Y);
        // _square.modify(&scale, 1.0 / columns, AXIS_X);
        // _actor->setRenderObj((Geo_RenderObj*)&_square);
    }
private:
    // Geo_FlatSquare _square = Geo_FlatSquare(1.0f);
    Geo_Actor* _actor;
#ifdef RASTERON_H
    Rasteron_Image* _image = nullptr; // background image available if Rasteron is supported
#endif
    unsigned _bkColor = 0xFF000000; // starting background is black
};

class Geo_PaneLayout : public Geo_Tree {
public:
    Geo_PaneLayout(
        const std::string& prefix,
        Topl_Scene* scene,
        unsigned rows,
        unsigned columns
	) : Geo_Tree(prefix, scene, &_squareGeo, (rows * columns) + 1) {
        _rows = rows;
        _columns = columns;
        fill(scene);
    }
private:
    void fill(Topl_Scene* scene) override;

	static Geo_FlatSquare _square;
	static Geo_Actor _squareGeo;
    Geo_Pane _rootPane = Geo_Pane(&_squareGeo); // all panes are children positioned relative to the root pane

    std::vector<Geo_Pane> _panes;
    unsigned _rows; 
    unsigned _columns;
};