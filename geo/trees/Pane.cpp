#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_square = Geo_FlatSquare(0.5f);
Geo_Actor Geo_PaneLayout::_squareGeo = Geo_Actor((Geo_RenderObj*)&_square);

namespace _Pane {
    std::string genPaneName(unsigned num){ return "pane" + std::to_string(num); }
}

void Geo_PaneLayout::fill(Topl_Scene* scene){
    Geo_Actor* actor = nullptr;

    Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane
    scene->addGeometry(getPrefix() + "root", rootActor);

    for(unsigned p = 1; p < getActorCount(); p++){
        actor = getNextActor();

        actor->setPos(Eigen::Vector3f(0.0f, DEFAULT_Y_INC * p, -1.0f * DEFAULT_Z_INC * p));
        // actor->setRot(Eigen::Vector2f(0.0f, DEFAULT_Y_INC * p)); // for testing
		scene->addGeometry(getPrefix() + _Pane::genPaneName(p), actor);

        _panes.push_back(Geo_Pane(actor));
		// _panes.back.scalePane(_rows, _columns);
    }
}