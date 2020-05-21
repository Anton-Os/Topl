#include "Topl_SceneGraph.hpp"

// More complex types

unsigned Topl_Node::mId_count = 0;

const Topl_GeoNode* const Topl_SceneGraph::getGeoNode(unsigned index) const {
    // std::map<const char*, unsigned>::iterator nameToId_iter;
    // nameToId_iter = mNameToId_map.find(name);

    return nullptr;
}

tpl_gEntity_cptr Topl_SceneGraph::getGeoNode(const std::string& name) const {

    if(mNameToId_map.find(name) == mNameToId_map.end()){
        puts("Name provided cannot be found");
        return nullptr;
    } 

    unsigned gIndex = mNameToId_map.at(name);
    return mIdToGeo_map.at(gIndex);
}

void Topl_SceneGraph::addGeometry(const std::string& name, tpl_gEntity_cptr GeoNode){
    if(mNameToId_map.find(name) != mNameToId_map.end()){
        puts("Duplicate names not allowed in scene graph");
        return;
    }

    mNameToId_map.insert({ name, ((Topl_Node*)GeoNode)->getId() });
    mIdToGeo_map.insert({ ((Topl_Node*)GeoNode)->getId(), GeoNode  });
}