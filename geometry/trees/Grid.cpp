#include "Grid.hpp"

namespace _Grid {
    std::string genCellName(unsigned num){ return "cell" + std::to_string(num); }
}

Geo_Grid_Params::Geo_Grid_Params(gridAttrib_pair attribs){
    xAttr = attribs; yAttr = attribs; zAttr = attribs;
    clamp();
}

Geo_Grid_Params::Geo_Grid_Params(gridAttrib_pair xA, gridAttrib_pair yA, gridAttrib_pair zA){
    xAttr = xA; yAttr = yA; zAttr = zA;
    clamp();
}

void Geo_Grid_Params::clamp(){
    if(xAttr.first == 0) xAttr.first++;
	if(yAttr.first == 0) yAttr.first++;
	if(zAttr.first == 0) zAttr.first++;
}

// Grid Class Code

void Geo_Grid::configure(Topl_Scene* scene){
    Geo_Actor* actor = nullptr;
    Vec3f offset = Vec3f((Vec3f){ 0.0f, 0.0f, 0.0f });

    const unsigned short width = properties.xAttr.first;
    const float x = properties.xAttr.second;
    const unsigned short height = properties.yAttr.first;
    const float y = properties.yAttr.second;
    const unsigned short depth = properties.zAttr.first;
    const float z = properties.zAttr.second;
    
    for(unsigned c = 0; c < getActorCount(); c++){
        actor = getNextActor();

		offset = Vec3f((Vec3f){
			((c % width) * x),
			((c % (height * width)) / width) * y,
			(c / (height * width)) * z
        });

		actor->updatePos(origin + offset);
        scene->addGeometry(getPrefix() + _Grid::genCellName(c + 1), actor);
		scene->addPhysics(getPrefix() + _Grid::genCellName(c + 1), &phys.at(c));
    }
}