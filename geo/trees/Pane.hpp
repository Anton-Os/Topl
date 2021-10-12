#include "Geo_Flat.hpp"
#include "Geo_Tree.hpp"

struct Geo_Pane {
    Geo_Pane(const Geo_FlatSquare* s, const Geo_Actor* g){ 
        square = s;
        squareGeo = g;
    }

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
    ) : Geo_Tree(prefix, scene, &squareGeo, paneCount + 1){ // includes the parent pane as well
        fill(scene);
    } 
private:
    void fill(Topl_Scene* scene) override;

    Geo_FlatSquare square = Geo_FlatSquare(1.0f);
    Geo_Actor squareGeo = Geo_Actor((Geo_RenderObj*)&square);
    Geo_Pane parent = Geo_Pane(&square, &squareGeo);
};