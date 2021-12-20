#include "primitives/Geo_Flat.hpp"
#include "Geo_Tree.hpp"

#define DEFAULT_Y_INC 0.01
#define DEFAULT_Z_INC 0.0001

class Geo_Pane {
public:
    Geo_Pane(const Geo_Actor* a){ 
        _actor = a;
        // _renderObj = *(a->getRenderObj());
        _bkColor = (a->getId() % 2)? 0xFFFF0000 : 0xFF0000FF; // switch to red or blue background
    }

private:
    // Geo_RenderObj _renderObj = (Geo_RenderObj)Geo_FlatSquare(1.0f);
    const Geo_Actor* _actor;
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
        unsigned paneCount
    ) : Geo_Tree(prefix, scene, &_squareGeo, paneCount + 1){ // includes the root pane as well
        fill(scene);
    }
private:
    void fill(Topl_Scene* scene) override;

	static Geo_FlatSquare _square;
	static Geo_Actor _squareGeo;
    Geo_Pane _rootPane = Geo_Pane(&_squareGeo); // all panes are children positioned relative to the root pane

    std::vector<Geo_Pane> _panes;
};