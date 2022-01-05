#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_dummy = Geo_FlatSquare(DEFAULT_PANE_SIZE);
Geo_Actor Geo_PaneLayout::_dummyGeo = Geo_Actor((Geo_RenderObj*)&_dummy);

namespace _Pane {
    std::string genPaneName(unsigned num){ return "pane" + std::to_string(num); }
}

void Geo_PaneLayout::fill(Topl_Scene* scene){
    Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane
    scene->addGeometry(getPrefix() + "root", rootActor);

    Geo_Actor* actor = nullptr; // keeps track of child actor being processed after root
    for(unsigned p = 1; p < getActorCount(); p++){
        actor = getNextActor();
        // pane modification
#ifdef RASTERON_H
        // scene->addTexture(getPrefix() + _Pane::genPaneName(p), _panes[p - 1].getBackground());
#endif
        // positioning
        actor->setPos(Eigen::Vector3f(0.0f, DEFAULT_Y_INC * p, -1.0f * DEFAULT_Z_INC * p)); // adjust these values
		// actor->setRot(Eigen::Vector2f(0.1f, 0.1f)); // for testing
        scene->addGeometry(getPrefix() + _Pane::genPaneName(p), actor);
    }
}