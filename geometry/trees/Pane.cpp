#include "Pane.hpp"

Geo_FlatSquare Geo_PaneLayout::_dummy = Geo_FlatSquare(PANE_RADIUS);
Geo_Actor Geo_PaneLayout::_dummyGeo = Geo_Actor((Geo_RenderObj*)&_dummy);

namespace _Pane {
    std::string genPaneName(unsigned num){ return "pane" + std::to_string(num); }
}

void Geo_PaneLayout::fill(Topl_Scene* scene){
    Geo_Actor* rootActor = getNextActor(); // 1st actor is root pane
    rootActor->setRenderObj((Geo_RenderObj*)&_rootSquare);
    scene->addGeometry(getPrefix() + "root", rootActor);
#ifdef RASTERON_H
	scene->addTexture(getPrefix() + "root", _rootPane.getBackground());
#endif

    Geo_Actor* actor = nullptr; // keeps track of child actor being processed after root
    for(unsigned p = 1; p < getActorCount(); p++){
        actor = getNextActor();
        actor->setRenderObj((Geo_RenderObj*)&_childSquare);

        // positioning
        Eigen::Vector2f origin = Eigen::Vector2f(PANE_RADIUS * -0.5, PANE_RADIUS * 0.5);
        float xInc = (PANE_RADIUS * 2) / _columns;
        float yInc = (PANE_RADIUS * 2) / _rows;
        unsigned short xOffset = (p - 1) % _columns;
        unsigned short yOffset = (p - 1) / _columns;

        // actor->setPos(Eigen::Vector3f(origin.x() + (xInc * xOffset), origin.y() - (yInc * yOffset), PANE_Z)); // adjust these values
        actor->setPos(Eigen::Vector3f(xInc * xOffset, -1.0 * yInc * yOffset, PANE_Z)); // adjust these values
		// actor->setRot(Eigen::Vector2f(0.1f, 0.1f)); // for testing
        scene->addGeometry(getPrefix() + _Pane::genPaneName(p), actor);
#ifdef RASTERON_H
		scene->addTexture(getPrefix() + _Pane::genPaneName(p), _panes[p - 1].getBackground());
#endif
    }
}