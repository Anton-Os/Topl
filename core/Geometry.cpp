#include "Geometry.hpp"

#include <cmath>
// #include <math.h>
#include <cstdlib>
#include <cstdio>

// Geo_Rect2D Implementation

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

Eigen::Vector3f* Geo_Sphere2D::genVertices(){
    Eigen::Vector3f* data = (Eigen::Vector3f*)malloc(mVCount * sizeof(Eigen::Vector3f));

	const double fullAngle = TOPL_PI * 2;
	const double incAngle = fullAngle / mCircle.segments;
	const double startAngle = fullAngle / 4; // Start at 90 degrees, pointing vertically

    *(data + 0) = Eigen::Vector3f(0.0f, 0.0f, 0.0f); // Circle origin
    for(unsigned v = 1; v < mVCount; v++) // We have already created
        *(data + v) = Eigen::Vector3f(
								sin(startAngle + (v * incAngle)) * mCircle.radius, 
								cos(startAngle + (v * incAngle)) * mCircle.radius, 
								0.5f);

    return data;
}

unsigned* Geo_Sphere2D::genIndices(){
    unsigned* data = (unsigned*)malloc(mICount * sizeof(unsigned));

	unsigned startCVert = 1; // I increment this after every assignment
	for (unsigned i = 0; i < mICount; i += 3) { // We are setting 3 indices in one go
		*(data + i + 0) = 0; // Origin point
		*(data + i + 1) = startCVert; // Take the start vertex
		*(data + i + 2) = startCVert + 1; // Connect to next vertex

		startCVert++;
	}

    return data;
}

// Geo_Sphere2D Implementation




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