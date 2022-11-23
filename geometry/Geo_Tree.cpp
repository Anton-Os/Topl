#include "Geo_Tree.hpp"

Geo_Tree::Geo_Tree(const std::string& prefix, std::initializer_list<Geo_Renderable*> renderObjs){
    _prefix = prefix;
    _actorData = (Geo_Actor**)malloc(renderObjs.size() * sizeof(Geo_Actor*));
    for(std::initializer_list<Geo_Renderable*>::iterator renderObj = renderObjs.begin(); renderObj < renderObjs.end(); renderObj++){
        *(_actorData + _actorCount) = new Geo_Actor(*(renderObj));
        _actorCount++;
    }
}

Geo_Tree::Geo_Tree(const std::string& prefix, const Geo_Actor* actor, unsigned count){
    _prefix = prefix;
    _actorCount = count;
    _actorData = (Geo_Actor**)malloc(count * sizeof(Geo_Actor));

    for(unsigned g = 0; g < count; g++) 
        *(_actorData + g) = new Geo_Actor(*actor);
}

Geo_Tree::~Geo_Tree(){
    if(_actorData != nullptr) { 
        for (unsigned g = 0; g < _actorCount; g++)
            delete *(_actorData + g);
        free(_actorData);
    }
}

void Geo_Tree::rotate(Vec3f angles){
    rotateAll(angles);

    Vec3f origin = getOrigin();
    for(unsigned a = 0; a < _actorCount; a++){
        Geo_Actor* actor =  *(_actorData + a);
        Vec3f actor_pos = actor->getPosition() - origin;
        
        float x = (actor_pos[0] * cos(angles[0])) - (actor_pos[1] * sin(angles[0]));
        float y = (actor_pos[0] * sin(angles[0])) + (actor_pos[1] * cos(angles[0]));
        float z = actor_pos[2];
        actor_pos = Vec3f({ x, y, z });
        actor->setPos(origin + actor_pos);
    }
}

Vec3f Geo_Tree::getOrigin(){
    Vec3f origin = Vec3f({ 0.0f, 0.0f, 0.0f });
    for(unsigned a = 0; a < _actorCount; a++){
        Geo_Actor* actor =  *(_actorData + a);
        origin = origin + actor->getPosition();
    }
    origin = Vec3f({ origin[0] / _actorCount, origin[1] / _actorCount, origin[2] / _actorCount });
    return origin;
}

Geo_Actor* Geo_Tree::getNextActor(){
    if(_actorOffset <= _actorCount){
        _actorOffset++;   
        return *(_actorData + _actorOffset - 1); // to increment offset above in one line
    } else return nullptr; // error has occured here
}