// More Complex types
#ifndef TOPL_SCENE_MANAGER_H

#include "native_os_def.h" // This includes the Rasteron.h header, TODO Fix This!!!

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <cmath>

#include "FileIO.hpp" // REMOVE THIS!!! DEBUGGING ONLY!!!
#include "Timer.hpp"
#include "Physics.hpp"

#include "composites/Geo_Component.hpp"

typedef const Geo_Component* const topl_geoComponent_cptr;
typedef std::pair<const Geo_Component*, const Geo_Component*> geoComponent_pair;

struct LinkedItems { // Wrapper around a physics connector and the two objects being linked
	Phys_Connector* connector;
	geoComponent_pair linkedItems;
};

typedef const LinkedItems* const topl_linkedItems_cptr;

#ifdef RASTERON_H
	typedef std::pair<unsigned, const Rasteron_Image*> idToImage_pair;
#endif

class Topl_Camera {
public:
	vec3f_cptr getPos() const { return &pos; }
	vec3f_cptr getDirection() const { return &direction; }
private:
	Eigen::Vector3f pos = Eigen::Vector3f(0.0, 0.0, -1.0);
	Eigen::Vector3f direction = Eigen::Vector3f(0.0, 0.0, 0.0);;
};

typedef const Topl_Camera* const topl_camera_cptr;

// Scene Manager is essentially the singleton game object, everything passes through here to be renedered to the screen
// --------------------------------------------------------------------------------------------------------------------

class Topl_SceneManager {
public:
	Topl_SceneManager() {
		mPhysTicker.reset(); // Resets timer for dynamic scene manager operations
	}
	~Topl_SceneManager() {}

	void addGeometry(const std::string& name, Geo_Component* geoNode);
#ifdef RASTERON_H
	// TODO: Move definition to Topl_SceneManager.cpp and check for valid name input
	void addTexture(const std::string& name, const Rasteron_Image* rstnImage) {
		mIdToTex.push_back(std::make_pair(mNameToId_map.at(name), rstnImage));
	}
#endif

	void addForce(const std::string& name, const Eigen::Vector3f& vec);
	void addPhysics(const std::string& name, Phys_Properties* pProp);
	void addConnector(Phys_Connector* connector, const std::string& name1, const std::string& name2);
	void resolvePhysics(); // Iterates through all appropriate members in mIdToPhysProp_map

	unsigned getGeoCount() const { return mIdToGeo_map.size(); }
	topl_geoComponent_cptr getGeoNode(unsigned index) const; // Access to geometry sequentially
	topl_geoComponent_cptr getGeoNode(const std::string& name) const; // Access to geometry by name

	unsigned getLinkedItemsCount() const { return mLinkedItems.size(); }
	topl_linkedItems_cptr getLink(unsigned index) const; // Access to links sequentially

	topl_camera_cptr getCamera() const { return &mCamera; }
#ifdef RASTERON_H
	const Rasteron_Image* getFirstTexture(unsigned index) const;
	unsigned getTextures(unsigned index, const Rasteron_Image** images) const; // Sequential access, see MAX_BUFFERS_PER_TARGET in Renderer.hpp
#endif
private:
	std::map<std::string, unsigned> mNameToId_map; // Associates names to object by IDs
	std::map<unsigned, Geo_Component*> mIdToGeo_map;
	std::map<unsigned, Phys_Properties*> mIdToPhysProp_map;
	std::vector<LinkedItems> mLinkedItems;
	Timer_Ticker mPhysTicker; // This ticker is specific to physics updates

	Topl_Camera mCamera;

#ifdef RASTERON_H
	std::vector<idToImage_pair> mIdToTex; // Multiple textures could be associated to a geometry node
#endif
};

#define TOPL_SCENE_MANAGER_H
#endif