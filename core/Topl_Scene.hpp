// More Complex types
#ifndef TOPL_SCENE_H

#include <vector>
#include <map>
#include <string>

#include "support_def.h"

#include "Timer.hpp"
#include "ValueGen.hpp"

#include "Topl_Camera.hpp"
#include "Geo_Actor.hpp"

typedef const Geo_Actor* const geo_cptr;
typedef std::pair<const Geo_Actor*, const Geo_Actor*> geo_pair;

struct LinkedItems { // Wrapper around a physics connector and the two objects being linked
	Phys_Connector* connector;
	geo_pair linkedItems;
};

typedef const LinkedItems* const linkedItems_cptr;

struct Topl_LightSource {
	Topl_LightSource(Eigen::Vector3f p) {
		pos = p;
	}
	Topl_LightSource(Eigen::Vector3f p, Eigen::Vector3f lc, double i) { // Extended Constructor
		pos = p;
		lightColor = lc;
		intensity = i;
	}
	Eigen::Vector3f pos;
	Eigen::Vector3f lightColor;
	double intensity = 1.0;
};

typedef const Topl_LightSource* const lightSource_cptr; // typedef for safety

#ifdef RASTERON_H
	typedef std::pair<unsigned, const Rasteron_Image*> idToImage_pair;
#endif

// Scene Manager is essentially the singleton game object, everything passes through here to be renedered to the screen
// --------------------------------------------------------------------------------------------------------------------

class Topl_Scene {
public:
	Topl_Scene() {
		_physTicker.reset(); // Resets timer for dynamic scene manager operations
	}
	~Topl_Scene() {}

	void addGeometry(const std::string& name, Geo_Actor* geo);
	void addLightSource(Topl_LightSource ls){ _lightSrc.push_back(ls); }
#ifdef RASTERON_H
	void addTexture(const std::string& name, const Rasteron_Image* rastImage);
#endif
	unsigned getActorCount() const { return _namedActor.size(); }
	geo_cptr getGeoActor(unsigned index) const; // access to geometry by index
	geo_cptr getGeoActor(const std::string& name) const; // access to geometry by name
	unsigned getLightSourceCount() const { return _lightSrc.size(); }
	lightSource_cptr getLightSource(unsigned index) const; // access to light source by index
#ifdef RASTERON_H
	const Rasteron_Image* getFirstTexture(const std::string& name) const;
	unsigned getTextures(unsigned index, const Rasteron_Image** images) const; // Sequential access, see MAX_BUFFERS_PER_TARGET in Renderer.hpp
#endif

	void addForce(const std::string& name, const Eigen::Vector3f& vec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addConnector(Phys_Connector* connector, const std::string& name1, const std::string& name2);
	void modConnector(const std::string& targetName, Eigen::Vector3f rotAnglesVec, double lengthScale); // Rotates and scales all connectors associated with named geometry
	void remConnector(const std::string& targetName); // Breaks all connectors associated with named geometry
	void resolvePhysics(); // Iterates through all appropriate members in _idToPhysProp_map

	unsigned getLinkedItemsCount() const { return _linkedItems.size(); }
	linkedItems_cptr getLink(unsigned index) const; // Access to links sequentially
private:
	Topl_Camera _camera;

	std::vector<Geo_Actor*> _namedActor; // Stores all geometries
	std::vector<Topl_LightSource> _lightSrc; // Stores all light sources
	std::map<Geo_Actor*, Phys_Actor*> _geoToPhys_map; // Associates geometry to a physics structure
	std::vector<LinkedItems> _linkedItems; // Stores geometry connector data
	std::vector<Phys_Colliders> _colliders; // Stores physics data specific to collision
	Timer_Ticker _physTicker; // This ticker is specific to physics updates
	// const Platform* _platform; // Provides useful system information and parameters
#ifdef RASTERON_H
	std::map<Geo_Actor*, const Rasteron_Image*> _geoTex_map; // Associates geometry to a single texture structure
#endif
};

#define TOPL_SCENE_H
#endif