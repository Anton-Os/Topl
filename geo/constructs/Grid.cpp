#include "Grid.hpp"

namespace _Grid {
    std::string genCellName(unsigned num){ return "cell" + std::to_string(num); }
}

void Geo_Grid::fill(Topl_Scene* scene){
    Geo_Actor* currentGeo = nullptr;
    Eigen::Vector3f offsetVec = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

    unsigned short width = grid_prop.xAttr.first;
    unsigned short height = grid_prop.yAttr.first;
    unsigned short depth = grid_prop.zAttr.first;
    
    for(unsigned c = 0; c < getGeoCount(); c++){
        currentGeo = getNextGeo();

		offsetVec = Eigen::Vector3f(
			(c % width) * grid_prop.xAttr.second,
			((c % (height * width)) / width) * grid_prop.zAttr.second,
			(c / (height * width)) * grid_prop.zAttr.second
		);
        currentGeo->updatePos(originPos + offsetVec);
        scene->addGeometry(getPrefix() + _Grid::genCellName(c + 1), currentGeo);
		scene->addPhysics(getPrefix() + _Grid::genCellName(c + 1), &phys.at(c));
    }
}

void Geo_Grid::updateScene(Topl_Scene* scene) {
    _updateCount++;
    return;
} 