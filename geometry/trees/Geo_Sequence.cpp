#include "Geo_Sequence.hpp"

static std::string genLinkName(unsigned num){ return "link" + std::to_string(num); }

void Geo_Sequence::configure(Topl_Scene* scene){
    Geo_Actor* prevActor = nullptr;
    Geo_Actor* actor = nullptr;
    
    for(unsigned c = 0; c < getActorCount(); c++){
        actor = getNextActor();

		actor->updatePos(_origin + (_params.direction * c));
        scene->addGeometry(getPrefix() + genLinkName(c + 1), actor);
        scene->addPhysics(getPrefix() + genLinkName(c + 1), &physActors.at(c));

        if(prevActor != nullptr){ // first link is ignored because it has no previous geo to link to
            links.push_back(Phys_Connector());
            scene->addLink(&links.back(), getPrefix() + genLinkName(c), getPrefix() + genLinkName(c + 1));
        }

        prevActor = actor; // moves previous link up the Sequence
    }

}