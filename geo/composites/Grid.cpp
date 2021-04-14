#include "Grid.hpp"

namespace _Grid {
    std::string genCellName(unsigned num){
        return "cell" + std::to_string(num);
    }
}

void Geo_Grid::fill(Topl_Scene* scene){
    Geo_Component* currentGeoc = nullptr;
    Eigen::Vector3f offsetVec = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

    unsigned short width = grid_prop.widthVals.first;
    unsigned short height = grid_prop.heightVals.first;
    unsigned short depth = grid_prop.depthVals.first;
    
    for(unsigned c = 0; c < getGeoCount(); c++){
        currentGeoc = getNextGeo();

		offsetVec = Eigen::Vector3f(
			(c % width) * grid_prop.widthVals.second,
			((c % (height * width)) / width) * grid_prop.depthVals.second,
			(c / (height * width)) * grid_prop.depthVals.second
		);
        currentGeoc->updatePos(bottomLeftCorner + offsetVec);
        scene->addGeometry(getPrefix() + _Grid::genCellName(c + 1), currentGeoc);
    }
}

void Geo_Grid::updateSceneManager(Topl_Scene* scene) {
    mUpdateCount++;
    return;
} 