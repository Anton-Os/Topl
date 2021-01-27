#include "Grid.hpp"

namespace _Grid {
    std::string genCellName(unsigned num){
        return "cell" + std::to_string(num);
    }
}

void Geo_Grid::fill(Topl_SceneManager* sMan){
    Geo_Component* currentGeoc = nullptr;
    
    for(unsigned c = 0; c < getGeoCount(); c++){
        currentGeoc = getNextGeo();

        currentGeoc->updatePos(bottomCorner); // TODO: Add appropriate size vector to create a grid
        sMan->addGeometry(getPrefix() + _Grid::genCellName(c + 1), currentGeoc);
    }
}

void Geo_Grid::updateSceneManager(Topl_SceneManager* sMan) {
    mUpdateCount++;
    return;
} 

void Geo_Grid::move(Topl_SceneManager* sMan, Eigen::Vector3f vec){
	return;
}

void Geo_Grid::rotate(Topl_SceneManager* sMan, Eigen::Vector3f vec){
	return;
}