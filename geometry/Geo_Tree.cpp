#include "Geo_Tree.hpp"

Geo_Tree::Geo_Tree(const std::string& prefix, std::initializer_list<Geo_RenderObj*> renderObjs){
    _prefix = prefix;
    _actorData = (Geo_Actor**)malloc(renderObjs.size() * sizeof(Geo_Actor*));
    for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
        *(_actorData + _actorCount) = new Geo_Actor(*(currentRenderObj));
        _actorCount++;
    }
}

Geo_Tree::Geo_Tree(const std::string& prefix, const Geo_Actor* actor, unsigned count){
    _prefix = prefix;
    _actorCount = count;
    _actorData = (Geo_Actor**)malloc(count * sizeof(Geo_Actor));

    for(unsigned g = 0; g < count; g++) *(_actorData + g) = new Geo_Actor(*actor);
}

Geo_Tree::~Geo_Tree(){
    if(_actorData != nullptr) { 
        for (unsigned g = 0; g < _actorCount; g++)
            delete *(_actorData + g);
        free(_actorData);
    }
}

void Geo_Tree::rotate(const Eigen::Vector2f& angles){
    rotateAll(angles);

    Eigen::Vector3f origin = getOrigin();
    for(unsigned a = 0; a < _actorCount; a++){
        Geo_Actor* actor =  *(_actorData + a);
        Eigen::Vector3f actor_pos = *(actor->getPos()) - origin;
        // Eigen::Vector2f actor_angles = *(actor->getAngles());
        
        double x = (actor_pos.x() * cos(angles[0])) - (actor_pos.y() * sin(angles[0]));
        double y = (actor_pos.x() * sin(angles[0])) + (actor_pos.y() * cos(angles[0]));
        double z = actor_pos.z();
        actor_pos = Eigen::Vector3f(x, y, z);
        actor->setPos(origin + actor_pos);
    }
}

Eigen::Vector3f Geo_Tree::getOrigin(){
    Eigen::Vector3f origin = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    for(unsigned a = 0; a < _actorCount; a++){
        Geo_Actor* actor =  *(_actorData + a);
        origin += *actor->getPos();
    }
    origin = Eigen::Vector3f(origin.x() / _actorCount, origin.y() / _actorCount, origin.z() / _actorCount);
    return origin;
}

Geo_Actor* Geo_Tree::getNextActor(){
    if(_actorOffset <= _actorCount){
        _actorOffset++;   
        return *(_actorData + _actorOffset - 1); // to increment offset above in one line
    } else return nullptr; // error has occured here
}