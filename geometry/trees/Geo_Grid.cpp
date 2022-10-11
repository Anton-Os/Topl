#include "Geo_Grid.hpp"

static std::string genCellName(unsigned num){ return "cell" + std::to_string(num); }

Geo_Grid_Params::Geo_Grid_Params(gridAttrib_pair xyzAttrib){
    x = xyzAttrib; 
	y = xyzAttrib; 
	z = xyzAttrib;
    clamp();
}

Geo_Grid_Params::Geo_Grid_Params(gridAttrib_pair xAttrib, gridAttrib_pair yAttrib, gridAttrib_pair zAttrib){
    x = xAttrib; 
	y = yAttrib; 
	z = zAttrib;
    clamp();
}

void Geo_Grid_Params::clamp(){
    if(x.first == 0) x.first++;
	if(y.first == 0) y.first++;
	if(z.first == 0) z.first++;
}

// Grid operations

void Geo_Grid::configure(Topl_Scene* scene){
    Geo_Actor* actor = nullptr;
    Vec3f offset = Vec3f({ 0.0f, 0.0f, 0.0f });

    const unsigned short width = _params.x.first;
    const float x = _params.x.second;
    const unsigned short height = _params.y.first;
    const float y = _params.y.second;
    const unsigned short depth = _params.z.first;
    const float z = _params.z.second;
    
    for(unsigned c = 0; c < getActorCount(); c++){
        actor = getNextActor();

		offset = Vec3f({
			((c % width) * x),
			((c % (height * width)) / width) * y,
			(c / (height * width)) * z
        });

		actor->updatePos(_origin + offset);
        scene->addGeometry(getPrefix() + genCellName(c + 1), actor);
		scene->addPhysics(getPrefix() + genCellName(c + 1), &physActors.at(c));
    }
}