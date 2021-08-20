#include "Geo_Flat.hpp"
#include "Geo_Construct.hpp"

struct Geo_Pane_Properties {
    double xOffFrac = 0.0; // x offset fraction
    double yOffFrac = 0.0; // y offset fraction
    double widthFrac = 1.0; // width portion of parent to fill
    double heightFrac = 1.0; // height portion of parent to fill
#ifdef RASTERON_H
    Rasteron_Image* image;
#else
    unsigned bkColor = 0xFFFFFF00; // yellow background color by default
#endif
}

#define ROOT_PANE_LEVEL 0

class Geo_Pane {
    Geo_Pane(const Geo_Pane_Properties* props, unsigned short level){
        pane_level = level;
        pane_prop = *props;
    }

    unsigned short pane_level;
    Geo_Pane_Properties pane_prop;

    // Geo_FlatSquare square;
    // Geo_Component geo;
}

/* class Geo_PaneTree : public Geo_Construct {
public:
    Geo_Pane( // Root pane constructor without parent
        const std::string& name,
        Topl_Scene* scene,
        const Geo_Pane* parentPane,
        std::vector<const Geo_Pane*> childPanes,
    ) : Geo_Construct(name, scene, childPanes.size() + 1) { // size includes child count plus current pane
        fillScene();
    }

} */