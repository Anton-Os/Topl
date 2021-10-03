#include "Chain.hpp"

namespace _Chain {
    std::string genLinkName(unsigned num){ return "link" + std::to_string(num); }
}

void Geo_Chain::fill(Topl_Scene* scene){
    Geo_Actor* prevGeo = nullptr;
    Geo_Actor* currentGeo = nullptr;
    
    for(unsigned c = 0; c < getGeoCount(); c++){
        currentGeo = getNextGeo();

        // currentGeo->updatePos(Eigen::Vector3f(chain_props.distance * c, 0.0f, 0.0f)); // TODO: Make this configurable!
        currentGeo->updatePos(chain_props.directionVec * c);
        scene->addGeometry(getPrefix() + _Chain::genLinkName(c + 1), currentGeo);
        scene->addPhysics(getPrefix() + _Chain::genLinkName(c + 1), &phys.at(c));

        if(prevGeo != nullptr){ // first link is ignored because it has no previous geo to link to
            links.push_back(Phys_Connector());
            scene->addConnector(&links.back(), getPrefix() + _Chain::genLinkName(c), getPrefix() + _Chain::genLinkName(c + 1));
        }

        prevGeo = currentGeo; // moves previous link up the chain
    }
}

void Geo_Chain::updateScene(Topl_Scene* scene) {
    _updateCount++;
    return;
} 