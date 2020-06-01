#include "Topl_SceneGraph.hpp"

void Topl_GeoNode::updatePos(Eigen::Vector3f vec){
    mRelWorldPos += vec;
    for(unsigned c = 0; c < getChildCount(); c++)
        ((Topl_GeoNode*)getChild(c))->updatePos(vec); // Move all children as well
}


// More complex types

unsigned Topl_Node::mId_count = 0;

tpl_gEntity_cptr Topl_SceneGraph::getGeoNode(unsigned index) const {
    if(mIdToGeo_map.find(index) == mIdToGeo_map.end()){
        puts("Index provided cannot be found");
        return nullptr;
    }

    return mIdToGeo_map.at(index);
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
        puts("Overriding geometry object:");
        puts(name.c_str());
    }

    mNameToId_map.insert({ name, ((Topl_Node*)GeoNode)->getId() });
    mIdToGeo_map.insert({ ((Topl_Node*)GeoNode)->getId(), GeoNode  });
}

/* void Topl_SceneGraph::updateGeoPos(const Eigen::Vector3f* pos){
    mRelWorldPos += pos;
} */