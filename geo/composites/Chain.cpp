#include "Chain.hpp"

namespace _Chain {
    std::string genLinkName(unsigned num){
        return "link" + std::to_string(num);
    }
}

void Geo_Chain::fill(Topl_SceneManager* sMan){
    Geo_Component* prevGeoc = nullptr;
    Geo_Component* currentGeoc = nullptr;
    
    for(unsigned c = 0; c < getGeoCount(); c++){
        currentGeoc = getNextGeo();

        currentGeoc->updatePos(Eigen::Vector3f(chainProp.distance * c, chainProp.distance * c, 0.0f));
        sMan->addGeometry(getPrefix() + _Chain::genLinkName(c + 1), currentGeoc);

        if(prevGeoc != nullptr){ 
            connectors.push_back(Phys_Connector());
            sMan->addConnector(&connectors.back(), getPrefix() + _Chain::genLinkName(c), getPrefix() + _Chain::genLinkName(c + 1));
        }

        prevGeoc = currentGeoc; // Move up the chain
    }
}

void Geo_Chain::updateSceneManager(Topl_SceneManager* sMan) {
    mUpdateCount++;
    return;
} 

void Geo_Chain::move(Topl_SceneManager* sMan, Eigen::Vector3f vec){
	return;
}

void Geo_Chain::rotate(Topl_SceneManager* sMan, Eigen::Vector3f vec){
	return;
}