#include "Chain.hpp"

namespace _Chain {
    std::string genLinkName(unsigned num){
        return "link" + std::to_string(num);
    }
}

void Geo_Chain::fill(Topl_Scene* scene){
    Geo_Component* prevGeoc = nullptr;
    Geo_Component* currentGeoc = nullptr;
    
    for(unsigned c = 0; c < getGeoCount(); c++){
        currentGeoc = getNextGeo();

        currentGeoc->updatePos(Eigen::Vector3f(chain_prop.distance * c, chain_prop.distance * c, 0.0f));
        scene->addGeometry(getPrefix() + _Chain::genLinkName(c + 1), currentGeoc);

        if(prevGeoc != nullptr){ 
            connectors.push_back(Phys_Connector());
            scene->addConnector(&connectors.back(), getPrefix() + _Chain::genLinkName(c), getPrefix() + _Chain::genLinkName(c + 1));
        }

        prevGeoc = currentGeoc; // Move up the chain
    }
}

void Geo_Chain::updateScene(Topl_Scene* scene) {
    mUpdateCount++;
    return;
} 