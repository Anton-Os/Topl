// More Complex types
#ifndef TOPL_SCENEGRAPH_H

#include "native_os_def.h" // This includes the Rasteron.h header, TODO Fix This!!!

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "Timer.hpp"
#include "Physics.hpp"

class Topl_Node { // Acts as a node
public:
	Topl_Node() {
		mId_count++;
		mId = mId_count;
	}
	~Topl_Node() {
		mId_count--;
	}

	void setName(const std::string& name) { mName = '_' + name; }
	void addChild(Topl_Node* child) { mChildren.push_back(child); }

	// const unsigned const* mId_ref = &mId;
	unsigned getId() const { return mId; }
	Topl_Node* getParent() const { return mParent; }
	Topl_Node* getChild(unsigned childNum) const {
		if (childNum > mChildren.size()) return nullptr;
		else return mChildren.at(childNum - 1);
	}
	unsigned getChildCount() { return mChildren.size(); }

private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id
	std::string mName = "_"; // Default is just an underscore

	Topl_Node* mParent = nullptr;
	std::vector<Topl_Node*> mChildren;
};


#include "Geometry.hpp"

class Topl_GeoNode : public Topl_Node {
public:
	Topl_GeoNode() : Topl_Node() {}
	Topl_GeoNode(const Geo_RenderObj* renderObj) : Topl_Node() { mRenderObj = renderObj; }

	void updatePos(Eigen::Vector3f vec); // Follow by more spatial update things

	vec3f_cptr getPos() const { return &mRelWorldPos; }
	// float getWeight() const { return mWeight; }

	const Geo_RenderObj* mRenderObj;
protected:
	enum GeoBehavior {
		GEO_Fixed = 0, // Should be the default
		GEO_Dynamic = 1
	} mBehavior;
private:
	Eigen::Vector3f mRelWorldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector3f mOrientAngl = Eigen::Vector3f(0.0, 0.0, 0.0); // Angles by which to rotate
	// float mWeight = 1.0f;
};

typedef unsigned geoUpdateFlags_t;

enum GEO_UpdateFlags {
	GEO_DataChange = 0,
	GEO_Transposed = 1,
	GEO_Rotated = 2,
};

typedef const Topl_GeoNode* const tpl_gEntity_cptr;
typedef std::pair<unsigned, Phys_Properties*> idToPhysProp_pair;
#ifdef RASTERON_H
typedef std::pair<unsigned, const Rasteron_Image*> idToImage_pair;
#endif

class Topl_SceneManager {
public:
	Topl_SceneManager() {
		mTicker.reset(); // Resets timer for dynamic scene manager operations
	}
	~Topl_SceneManager() {}

	void addGeometry(const std::string& name, Topl_GeoNode* geoNode);
#ifdef RASTERON_H
	// TODO: Move definition to Topl_SceneManager.cpp and check for valid name input
	void addTexture(const std::string& name, const Rasteron_Image* rstnImage) {
		mIdToTex.push_back(std::make_pair(mNameToId_map.at(name), rstnImage));
	}
#endif

	void addForce(const std::string& name, const Eigen::Vector3f& vec);
	void addPhysics(const std::string& name, Phys_Properties* pProp);
	void resolvePhysics(); // Iterates through all appropriate members in mIdToPhysProp_map
	void addConnector(const Phys_Connector* connector, const std::string& name1, const std::string& name2);

	unsigned getGeoCount() const { return mIdToGeo_map.size(); }
	tpl_gEntity_cptr getGeoNode(unsigned index) const; // For sequential access, beginning to end
	tpl_gEntity_cptr getGeoNode(const std::string& name) const; // Access to geometry by name
#ifdef RASTERON_H
	const Rasteron_Image* getFirstTexture(unsigned index) const;
	unsigned getTextures(unsigned index, const Rasteron_Image** images) const; // Sequential access, see MAX_BUFFERS_PER_TARGET in Renderer.hpp
#endif
private:
	std::map<std::string, unsigned> mNameToId_map; // Associates names to object by IDs
	std::map<unsigned, Topl_GeoNode*> mIdToGeo_map;
	std::map<unsigned, Phys_Properties*> mIdToPhysProp_map;

	Timer_Ticker mTicker;
#ifdef RASTERON_H
	std::vector<idToImage_pair> mIdToTex; // Multiple textures could be associated to a geometry node
#endif
};

#define TOPL_SCENEGRAPH_H
#endif