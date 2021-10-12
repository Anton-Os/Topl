#include "Geo_Flat.hpp"
#include "Geo_Tree.hpp"

/* struct Geo_Pane_Properties {
    double xOffFrac = 0.0; // x offset fraction
    double yOffFrac = 0.0; // y offset fraction
    double widthFrac = 1.0; // width portion of parent to fill
    double heightFrac = 1.0; // height portion of parent to fill
#ifdef RASTERON_H
    Rasteron_Image* image; // background image available if Rasteron is supported
#else
    unsigned bkColor = 0xFFFFFF00; // yellow background color by default
#endif
} */

struct Geo_Pane {
    Geo_Pane(const Geo_FlatSquare* s, const Geo_Actor* g){ 
        square = s;
        squareGeo = g;
    }

    /* Geo_Pane(const Geo_Pane_Properties* props){ 
        pane_prop = *props;
    } */

    Geo_FlatSquare* square;
    Geo_Actor* squareGeo;
    // Geo_Pane_Properties pane_prop;
#ifdef RASTERON_H
    Rasteron_Image* image = nullptr; // background image available if Rasteron is supported
#else
    unsigned bkColor = 0xFFFFFF00; // yellow background color by default
#endif
}

class Geo_PaneLayout : public Geo_Tree {
public:
    Geo_PaneLayout(
        const std::string& name,
        Topl_Scene* scene,
        unsigned paneCount,
    ) : Geo_Tree(prefix, scene, &squareGeo, paneCount + 1){ // includes 
        fillScene(scene);
    } 

    void updateScene(Topl_Scene* scene) override;
private:
    void fill(Topl_Scene* scene) override;

    Geo_FlatSquare square = Geo_FlatSquare(1.0f);
    Geo_Actor squareGeo = Geo_Actor((Geo_RenderObj*)&square);
    Geo_Pane parent = Geo_Pane(&square, &squareGeo);
};