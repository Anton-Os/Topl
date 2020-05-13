#include "Geometry.hpp"

#include <cstdio>

Eigen::Vector3f* Geo_Rect2D::genVertices(){
    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));
    
    // The depth position of the vector is arbitrary for now
    Eigen::Vector3f topRight(mRect.height / 2.0f, mRect.width / 2.0f, 0.5f);
    Eigen::Vector3f topLeft(mRect.height / 2.0f, -1 * (mRect.width / 2.0f), 0.5f);
    Eigen::Vector3f botRight(-1 * (mRect.width / 2.0f), mRect.height / 2.0f, 0.5f);
    Eigen::Vector3f botLeft(-1 * (mRect.width / 2.0f), -1 * (mRect.width / 2.0f), 0.5f); 

    *(data + 0) = botLeft;
    *(data + 1) = botRight;
    *(data + 2) = topLeft;
    *(data + 3) = topRight;

    return data;
}

unsigned* Geo_Rect2D::genIndices(){
    unsigned* data = (unsigned*)malloc(mICount * sizeof(unsigned));

    unsigned indicesArray[6] = { 
        0, 1, 2, 
        3, 2, 1
    };

    // Simply copies from array to to the allocated data
    for(unsigned i = 0; i < 6; i++)
        *(data + i) = indicesArray[i];

    return data;
}

// More complex types

unsigned Topl_BaseEntity::mId_count = 0;

void Topl_BaseEntity::updateLocation(Eigen::Vector3f vec){
    //mRelLocation = loc;
    return;
}

const Topl_GeoEntity* const Topl_SceneGraph::getGeoEntity(unsigned index) const {
    // std::map<const char*, unsigned>::iterator nameToId_iter;
    // nameToId_iter = mNameToId_map.find(name);

    return nullptr;
}

tpl_gEntity_cptr Topl_SceneGraph::getGeoEntity(const std::string& name) const {

    if(mNameToId_map.find(name) == mNameToId_map.end()){
        puts("Name provided cannot be found");
        return nullptr;
    } 

    unsigned gIndex = mNameToId_map.at(name);
    return mIdToGeo_map.at(gIndex);
}

void Topl_SceneGraph::addGeometry(const std::string& name, tpl_gEntity_cptr geoEntity){
    if(mNameToId_map.find(name) != mNameToId_map.end()){
        puts("Duplicate names not allowed in scene graph");
        return;
    }

    mNameToId_map.insert({ name, geoEntity->getId() });
    mIdToGeo_map.insert({ geoEntity->getId(), geoEntity  });
}