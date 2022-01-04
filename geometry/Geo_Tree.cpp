#include "Geo_Tree.hpp"

Geo_Tree::Geo_Tree(const std::string& prefix, Topl_Scene* scene, std::initializer_list<Geo_RenderObj*> renderObjs){
    _prefix = prefix;
    _actorData = (Geo_Actor**)malloc(renderObjs.size() * sizeof(Geo_Actor*));
    for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
        *(_actorData + _actorCount) = new Geo_Actor(*(currentRenderObj));
        _actorCount++;
    }
}

Geo_Tree::Geo_Tree(const std::string& prefix, Topl_Scene* scene, const Geo_Actor* geoc, unsigned count){
    _prefix = prefix;
    _actorCount = count;
    _actorData = (Geo_Actor**)malloc(count * sizeof(Geo_Actor));

    for(unsigned g = 0; g < count; g++) *(_actorData + g) = new Geo_Actor(*geoc);
}

Geo_Tree::~Geo_Tree(){
    if(_actorData != nullptr) { 
        for (unsigned g = 0; g < _actorCount; g++)
            delete *(_actorData + g);
        free(_actorData);
    }
}

Geo_Actor* Geo_Tree::getNextActor(){
    if(_actorOffset <= _actorCount){
        _actorOffset++;   
        return *(_actorData + _actorOffset - 1); // to increment offset above in one line
    } else return nullptr;
}