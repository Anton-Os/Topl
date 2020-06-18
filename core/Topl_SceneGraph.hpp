// More Complex types
#ifndef TOPL_SCENEGRAPH_H

#include <memory>
#include <vector>

class Topl_Node { // Acts as a node
public:
	Topl_Node() {
		mId_count++;
		mId = mId_count;
	}
	~Topl_Node() { mId_count--; }

    void addChild(Topl_Node* child){ mChildren.push_back(child); }

	// const unsigned const* mId_ref = &mId;
	unsigned getId() const { return mId; }
    Topl_Node* getParent() const { return mParent; }
    Topl_Node* getChild(unsigned childNum) const {
        if(childNum > mChildren.size()) return nullptr;
        else return mChildren.at(childNum - 1);
    }
    unsigned getChildCount(){ return mChildren.size(); } 

private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id

    Topl_Node* mParent = nullptr;
    std::vector<Topl_Node*> mChildren;
    // Topl_Node** mChild = nullptr;
};


#include "Geometry.hpp"

class Topl_GeoNode : Topl_Node {
public:    
    Topl_GeoNode(const Geo_RenderObj* renderObj) : Topl_Node() { mRenderObj = renderObj; }
    
    void updatePos(Eigen::Vector3f vec); // Follow by more spatial update things

    vec3f_cptr getPos() const { return &mRelWorldPos; }

    const Geo_RenderObj* mRenderObj;
private:
    enum GeoBehavior {
        GEO_Fixed = 1,
        GEO_Dynamic = 2
    } mBehavior;

	Eigen::Vector3f mRelWorldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector3f mObjOrientAngl = Eigen::Vector3f(0.0, 0.0, 0.0); // Angles by which to rotate
};

typedef unsigned geoUpdateFlags_t;

enum GEO_UpdateFlags {
	GEO_DataChange = 0,
	GEO_Transposed = 1,
	GEO_Rotated = 2,
};

typedef const Topl_GeoNode* const tpl_gEntity_cptr;

class Topl_SceneGraph {
public:
    Topl_SceneGraph(){}
    ~Topl_SceneGraph(){}

    void addGeometry(const std::string& name, Topl_GeoNode* geoNode);
    void addForce(const std::string& name, const Eigen::Vector3f& vec);
    // void updateGeoPos(const Eigen::Vector3f* pos);

    unsigned getGeoCount() const { return mIdToGeo_map.size(); }
    tpl_gEntity_cptr getGeoNode(unsigned index) const;
    tpl_gEntity_cptr getGeoNode(const std::string& name) const;
private:
    std::map<std::string, unsigned> mNameToId_map; // Associates names to object by IDs
    std::map<unsigned, Topl_GeoNode*> mIdToGeo_map;
    std::map<unsigned, geoUpdateFlags_t> mIdToUpdate_map;
};

#define TOPL_SCENEGRAPH_H
#endif