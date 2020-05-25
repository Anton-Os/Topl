// More Complex types
#ifndef TOPL_SCENEGRAPH_H

#include <memory>

class Topl_Node { // Acts as a node
public:
	Topl_Node() {
		mId_count++;
		mId = mId_count;
	}
	~Topl_Node() { mId_count--; }

	// const unsigned const* mId_ref = &mId;
	unsigned getId() const { return mId; }
    Topl_Node* getParent() const { return mParent; }
    Topl_Node* getChild(unsigned childNum) const {
        if(childNum > mChildCount) return nullptr;
        else return *(mChild + childNum - 1);
    }

    // ADD CHILD FUNCTION
private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id

    Topl_Node* mParent = nullptr;
    unsigned mChildCount = 0;
    Topl_Node** mChild = nullptr;

};


#include "Geometry.hpp"

class Topl_GeoNode : Topl_Node {
public:    
    Topl_GeoNode(const Geo_RenderObj* renderObj) : Topl_Node() { mRenderObj = renderObj; }
    
    vec3f_cptr getLocation() const { return &mRelWorldPos; }
	void updateLocation(Eigen::Vector3f vec) { mRelWorldPos = vec; } // Follow by more spatial update things

	const Geo_RenderObj* mRenderObj; // Trying to eliminate the mType
private:
    enum GeoBehavior {
        GEO_Fixed = 1,
        GEO_Dynamic = 2
    } mBehavior;

	Eigen::Vector3f mRelWorldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector3f mObjOrientAngl = Eigen::Vector3f(0.0, 0.0, 0.0); // Angles by which to rotate
};


typedef const Topl_GeoNode* const tpl_gEntity_cptr;

class Topl_SceneGraph {
public:
    Topl_SceneGraph(){}
    ~Topl_SceneGraph(){}

    void addGeometry(const std::string& name, tpl_gEntity_cptr geoNode);

    unsigned getGeoCount() const { return mIdToGeo_map.size(); }
    tpl_gEntity_cptr getGeoNode(unsigned index) const;
    tpl_gEntity_cptr getGeoNode(const std::string& name) const;
private:
    std::map<std::string, unsigned> mNameToId_map; // Associates names to object by IDs
    std::map<unsigned, tpl_gEntity_cptr> mIdToGeo_map;
};

#define TOPL_SCENEGRAPH_H
#endif