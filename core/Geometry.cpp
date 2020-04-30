#include "Geometry.hpp"

unsigned Topl_BaseEntity::mId_count = 0;

void Topl_SceneGraph::addGeometry(const char* name, const Topl_GeoEntity* geoEntity){
    // std::map<const char*, unsigned>::iterator nameToId_it = mNameToId_map.begin() + mKeyCount;
    // mNameToId_map.insert(name, static_cast<Topl_BaseEntity*>(geoEntity)->get_Id());
    mNameToId_map.insert({ name, geoEntity->get_Id() });
    mIdToObj_map.insert({ geoEntity->get_Id(), geoEntity});
}