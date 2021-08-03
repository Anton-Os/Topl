#include "composites/Web.hpp"

namespace _Web {
    std::string genAnchorName(unsigned num){ return "anchor" + std::to_string(num); }
}

void Geo_Web::fill(Topl_Scene* scene){
    Geo_Component* currentGeo = nullptr;

    currentGeo = getNextGeo();
    scene->addGeometry(getPrefix() + _Web::genAnchorName(1), currentGeo);
    scene->addPhysics(getPrefix() +  _Web::genAnchorName(1), &phys.at(0));

    for(unsigned a = 1; a < getGeoCount(); a++){
        currentGeo = getNextGeo();

        currentGeo->updatePos(savedOffsets.at(a));
        scene->addGeometry(getPrefix() + _Web::genAnchorName(a + 1), currentGeo);
        scene->addPhysics(getPrefix() +  _Web::genAnchorName(a + 1), &phys.at(a));
    }
}

void Geo_Web::updateScene(Topl_Scene* scene) {
    _updateCount++;
    return;
} 