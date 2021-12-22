#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_square = Geo_FlatSquare(0.5f);
Geo_Actor Geo_PaneLayout::_squareGeo = Geo_Actor((Geo_RenderObj*)&_square);

namespace _Pane {
    std::string genPaneName(unsigned num){ return "pane" + std::to_string(num); }
}

void Geo_PaneLayout::fill(Topl_Scene* scene){
    Geo_Actor* actor = nullptr;

    for(unsigned p = 0; p < getActorCount(); p++){
        actor = getNextActor();

        actor->updatePos(Eigen::Vector3f(0.0f, DEFAULT_Y_INC * p, DEFAULT_Z_INC * p));
		scene->addGeometry(getPrefix() + _Pane::genPaneName(p + 1), actor);
        _panes.push_back(Geo_Pane(actor));
    }
}