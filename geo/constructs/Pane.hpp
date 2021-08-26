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

class Geo_Pane {
    Geo_Pane(const Geo_Pane_Properties* props, unsigned short level){
        pane_level = level;
        pane_prop = *props;
    }

    Geo_Pane_Properties pane_prop;
}

class Geo_PaneLayout : public Geo_Construct {
public:
    Geo_PaneLayout(
        const std::string& name,
        Topl_Scene* scene,
        const Geo_Pane* parent,
        std::vector<const Geo_Pane*> children
    ) : Geo_Construct(prefix, scene, &squareGeo, children.size() + 1){
        fillScene(scene);
    } 

    void updateScene(Topl_Scene* scene) override;
private:
    void fill(Topl_Scene* scene) override;

    static Geo_FlatSquare square = Geo_FlatSquare(1.0f);
    static Geo_Component squareGeo = Geo_Component((RenderObj*)&square);
};