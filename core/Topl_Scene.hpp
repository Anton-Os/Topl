// More Complex types
#ifndef TOPL_SCENE_H

#include <map>

#include "support_def.h"

#include "Timer.hpp"
#include "Physics.h"

#include "Geo_Actor.hpp"

#ifdef RASTERON_H
#include "Topl_Images.h"
// FT_Library Topl_Image::freetypeLib = {}; // static member definition
#endif

typedef const Geo_Actor* const actor_cptr;
typedef std::pair<const Geo_Actor*, const Geo_Actor*> link_pair;
typedef std::pair<const Geo_Actor*, const Eigen::Vector3f*> anchor_pair;

struct LinkedItems { // Wrapper around connector with 2 actors being linked
	Phys_Connector* connector;
	link_pair linkedItems;
};
typedef const LinkedItems* const linkedItems_cptr;

struct AnchoredItems { // Wrapper around connector with 1 actor being anchored
	Phys_Connector* connector;
	anchor_pair anchoredItems;
};
typedef const AnchoredItems* const anchorItem_cptr;


struct Topl_Light {
	Topl_Light(Eigen::Vector3f p) { pos = p; } // Simple Constructor
	Topl_Light(Eigen::Vector3f p, Eigen::Vector3f lc) { // Extended Constructor
		pos = p;
		value = lc;
	}
	Eigen::Vector3f pos;
	Eigen::Vector3f value = Eigen::Vector3f(1.0f, 1.0f, 1.0f); // indicates both color and intensity
};
typedef const Topl_Light* const light_cptr; // typedef for safety


class Topl_Camera {
public:
	// Identity projection constructor
	Topl_Camera() { _projMatrix = Eigen::Matrix4f::Identity(); } // Identity matrix by default
	Topl_Camera(enum PROJECTION_Type projType, SpatialBounds3D bounds){
		_projMatrix = genProjMatrix(projType, bounds);
	}
	void setPos(const Eigen::Vector3f& pos){ _pos = pos; }
	void movePos(const Eigen::Vector3f& move){ _pos += move; }
	void setLookPos(const Eigen::Vector3f& lookPos){ _lookPos = lookPos; }
	vec3f_cptr getPos() const { return &_pos; }
	vec3f_cptr getLookPos() const { return &_lookPos; }
	mat4f_cptr getProjMatrix() const { return &_projMatrix; }
private:
	Eigen::Vector3f _pos = Eigen::Vector3f(0.0, 0.0, -1.0);
	Eigen::Vector3f _lookPos = Eigen::Vector3f(0.0, 0.0, 0.0);
	Eigen::Matrix4f _projMatrix = Eigen::Matrix4f::Zero();
};

typedef const Topl_Camera* const camera_cptr;

// Scene Manager is essentially the singleton game object, everything passes through here to be renedered to the screen
// --------------------------------------------------------------------------------------------------------------------

class Topl_Scene {
public:
	Topl_Scene() { _ticker.reset(); }
	~Topl_Scene() {}

	// Statics Section
	void addGeometry(Geo_Actor* actor); // add geometry
	void addGeometry(const std::string& name, Geo_Actor* actor); // add geometry and override name
	void addLight(Topl_Light* ls){ _lightSrc.push_back(ls); }
#ifdef RASTERON_H
	void addTexture(const std::string& name, const Rasteron_Image* image);
	void addMaterial(const std::string& name, const Topl_Material* material);
#endif
	unsigned getActorCount() const { return _geoActors.size(); }
	actor_cptr getGeoActor(unsigned index) const; // access to geometry by index
	actor_cptr getGeoActor(const std::string& name) const; // access to geometry by name
	unsigned getLightCount() const { return _lightSrc.size(); }
	light_cptr getLight(unsigned index) const; // access to light source by index
#ifdef RASTERON_H
	unsigned getTexCount() const { return _actorTex_map.size(); }
	const Rasteron_Image* getTexture(const std::string& name) const;
	unsigned getMaterialCount() const { return _actorMaterial_map.size(); }
	const Rasteron_Image* getTexture(const std::string& name, MATERIAL_Property property) const;
#endif

	// Dynamics Section
	void addForce(const std::string& name, const Eigen::Vector3f& vec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2); // links 2 named geometry actors
	void addAnchor(Phys_Connector* connector, const std::string& name, const Eigen::Vector3f* pos); // anchors target named geometry object
	void remConnector(const std::string& targetName); // Breaks all connectors associated with named geometry
	void resolvePhysics(); // Iterates through all appropriate members in _idToPhysProp_map
	// unsigned getLinkedItemsCount() const { return _linkedItems.size(); }
	// linkedItems_cptr getLink(unsigned index) const; // Access to links sequentially
private:
	Topl_Camera _camera;
	std::vector<Topl_Light*> _lightSrc; // stores all light sources
#ifdef RASTERON_H
	std::map<Geo_Actor*, const Rasteron_Image*> _actorTex_map; // associates geometry actor to a single texture
	// std::map<Geo_Actor*, const Topl_Image*> __actorTex_map; // associates geometry actor to a single texture
	std::map<Geo_Actor*, const Topl_Material*> _actorMaterial_map; // associates geometry actor to multiple textures
#endif
	std::vector<Geo_Actor*> _geoActors; // stores all geometries
	std::map<Geo_Actor*, Phys_Actor*> _actorPhys_map; // associates geometry to a physics structure
	std::vector<LinkedItems> _linkedItems; // stores linked actors and connectors
	std::vector<AnchoredItems> _anchoredItems; // stores anchored actors and connectors
	
	Timer_Ticker _ticker; // used for internal updates
};

#define TOPL_SCENE_H
#endif