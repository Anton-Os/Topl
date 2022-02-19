// More Complex types
#ifndef TOPL_SCENE_H

#include <vector>
#include <map>
#include <string>

#include "support_def.h"

#include "Timer.hpp"
#include "ValueGen.hpp"

#include "Geo_Actor.hpp"

typedef const Geo_Actor* const actor_cptr;
typedef std::pair<const Geo_Actor*, const Geo_Actor*> geo_pair;

struct LinkedItems { // Wrapper around a physics connector and the two objects being linked
	Phys_Connector* connector;
	geo_pair linkedItems;
};
typedef const LinkedItems* const linkedItems_cptr;

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

#ifdef RASTERON_H
#define MAX_TEXTURES_PER_ACTOR 12 // corresponds to MAX_TEXTURES_PER_TARGET in Topl_Renderer.hpp
#define ANIM_BACKGROUND 0xFFFFFF00 // overrides macro set inside Rasterons Animation.h to change background

struct Topl_MultiTex { // object for working with multiple textures, wraps around Rasteron_Animation
	Topl_MultiTex(std::string prefix, unsigned height, unsigned width){
		animation = allocNewAnim(prefix.c_str(), height, width, MAX_TEXTURES_PER_ACTOR);
	}
	Topl_MultiTex(std::string prefix, unsigned height, unsigned width, unsigned short frameCount){
		animation = allocNewAnim(prefix.c_str(), height, width, frameCount);
	}
	~Topl_MultiTex(){ deleteAnim(animation); }
	
	void addFrame(const Rasteron_Image *const refImg);
	Rasteron_Image* getFrameNamed(const std::string& name) const;

	Rasteron_Animation* animation;
	unsigned frameIndex = 0;
	bool isOverride = false; // switch to true when images begin to override one another
};
#endif

class Topl_Camera {
public:
	// Identity projection constructor
	Topl_Camera() { _projMatrix = Eigen::Matrix4f::Identity(); } // Identity matrix by default
	Topl_Camera(enum PROJECTION_Type projType, SpatialBounds3D bounds){
		if (projType == PROJECTION_Perspective) _projMatrix = ValueGen::genPerspectiveMatrix(bounds);
		else if(projType == PROJECTION_Ortho) _projMatrix = ValueGen::genOrthoMatrix(bounds);
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
	Topl_Scene() {
		_physTicker.reset(); // Resets timer for dynamic scene manager operations
	}
	~Topl_Scene() {}

	// Scene Builder
	void addGeometry(const std::string& name, Geo_Actor* geo);
	void addLight(Topl_Light* ls){ _lightSrc.push_back(ls); }
#ifdef RASTERON_H
	void addTexture(const std::string& name, const Rasteron_Image* image);
	void addMultiTex(const std::string& name, const Topl_MultiTex* multiTex);
#endif
	unsigned getActorCount() const { return _geoActors.size(); }
	actor_cptr getGeoActor(unsigned index) const; // access to geometry by index
	actor_cptr getGeoActor(const std::string& name) const; // access to geometry by name
	unsigned getLightCount() const { return _lightSrc.size(); }
	light_cptr getLight(unsigned index) const; // access to light source by index
#ifdef RASTERON_H
	unsigned getTexCount() const { return _actorTex_map.size(); }
	const Rasteron_Image* getTexture(const std::string& name) const;
	unsigned getMultiTexCount() const { return _actorMultiTex_map.size(); }
	const Rasteron_Image* getTexture(const std::string& name, unsigned frameIndex) const;
#endif

	// Scene Physics
	void addForce(const std::string& name, const Eigen::Vector3f& vec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addConnector(Phys_Connector* connector, const std::string& name1, const std::string& name2);
	void modConnector(const std::string& targetName, Eigen::Vector3f rotAnglesVec, double lengthScale); // rotates and scales all connectors associated with named geometry
	void remConnector(const std::string& targetName); // Breaks all connectors associated with named geometry
	void resolvePhysics(); // Iterates through all appropriate members in _idToPhysProp_map

	unsigned getLinkedItemsCount() const { return _linkedItems.size(); }
	linkedItems_cptr getLink(unsigned index) const; // Access to links sequentially
private:
	Topl_Camera _camera;
	std::vector<Topl_Light*> _lightSrc; // stores all light sources
#ifdef RASTERON_H
	std::map<Geo_Actor*, const Rasteron_Image*> _actorTex_map; // associates geometry actor to a single texture
	std::map<Geo_Actor*, const Topl_MultiTex*> _actorMultiTex_map; // associates geometry actor to multiple textures
#endif

	std::vector<Geo_Actor*> _geoActors; // stores all geometries
	std::map<Geo_Actor*, Phys_Actor*> _actorPhys_map; // associates geometry to a physics structure
	std::vector<LinkedItems> _linkedItems; // stores geometry connector data
	Timer_Ticker _physTicker; // this ticker is specific to physics updates
};

#define TOPL_SCENE_H
#endif