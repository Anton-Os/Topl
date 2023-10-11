#ifndef TOPL_SCENE_H

#include <map>

#include "image_wrapper.h"
#include "support_def.h"

#include "Phys_Actor.h"
#include "Phys_Connector.h"
#include "Phys_Motion.h"

#include "IO.hpp"
#include "Timer.hpp"
#include "ValueGen.hpp"

#include "Geo_Actor.hpp"

// Light

enum LIGHT_Type { 
	LIGHT_Sky, // uniform direction
	LIGHT_Flash, // pointing relative respect to camera
	LIGHT_Lamp // hovers in scene
};

struct Topl_Light {
	Topl_Light(const Vec3f& p) { pos = p; } // Simple Constructor
	Topl_Light(const Vec3f& p, const Vec3f& v) { // Extended Constructor
		pos = p;
		value = v;
	}
	Vec3f pos;
	Vec3f value = Vec3f({ 1.0f, 1.0f, 1.0f }); // indicates both color and intensity
};
typedef const Topl_Light* const light_cptr; // typedef for safety

// Camera

class Topl_Camera {
public:
	Topl_Camera() { // Identity matrix
		_projMatrix = MAT_4x4_IDENTITY;
		_projMatrix(2, 2) = -1; // flip z axis
	}
	Topl_Camera(enum PROJECTION_Type projType){ // Regular Bounds
		_projMatrix = genProjMatrix(projType, SpatialBounds3D());
	}
	Topl_Camera(enum PROJECTION_Type projType, float scaleFactor){ // Sized Bounds
		_projMatrix = genProjMatrix(projType, SpatialBounds3D(scaleFactor));
	}
	Topl_Camera(enum PROJECTION_Type projType, SpatialBounds3D bounds){ // Custom Bounds
		_projMatrix = genProjMatrix(projType, bounds);
	}
	void setPos(const Vec3f& pos){ _pos = pos; }
	void updatePos(const Vec3f& vec){ _pos = _pos + vec; }
	void setRotation(const Vec3f& vec){ _rotation = vec; }
	void updateRotation(const Vec3f& vec) { _rotation = _rotation + vec; }
	vec3f_cptr_t getPos() const { return &_pos; }
	vec3f_cptr_t getRot() const { return &_rotation; }
	mat4x4_cptr_t getProjMatrix() const { return &_projMatrix; }
private:
	Vec3f _pos = Vec3f({ 0.0f, 0.0f, -1.0f }); // in front of scene
	Vec3f _rotation = Vec3f({ 0.0f, 0.0f, 0.0f }); // default pointing forward
	Mat4x4 _projMatrix = MAT_4x4_IDENTITY;
};

typedef const Topl_Camera* const camera_cptr;

// Scene

class Topl_Scene {
public:
	Topl_Scene() { _ticker.reset(); } // Empty Constructor
	Topl_Scene(const std::string& filePath) { // File Load Constructor
		// loadFromFile(filePath); 
	}
	~Topl_Scene() {}

	void saveToFile(const std::string& fileName); // saves scene data to .tp file
	void loadFromFile(const std::string& filePath); // loads scene data from .tp file

	// Static Operations

	void addGeometry(Geo_Actor* actor); // add geometry
	void addGeometry(const std::string& name, Geo_Actor* actor); // add named geometry
	void addLight(const Topl_Light* l){ _lightSrc.push_back(l); }
#ifdef RASTERON_H
	void addTexture(const std::string& name, const Rasteron_Image* image);
	void addMaterialTex(const std::string& name, const Img_Material* material);
	void addVolumeTex(const std::string& name, const Img_Volume* volume);
#endif
	unsigned getActorCount() const { return _geoActors.size(); }
	actor_cptr getGeoActor(unsigned index) const; // access to geometry by index
	actor_cptr getGeoActor(const std::string& name) const; // access to geometry by name
	unsigned getLightCount() const { return _lightSrc.size(); }
	light_cptr getLight(unsigned index) const; // access to light source by index
#ifdef RASTERON_H
	unsigned getTexCount() const { return _actorTex_map.size(); }
	const Rasteron_Image* getTexture(const std::string& name) const;
	unsigned getMatCount() const { return _actorTex2D_map.size(); }
	const Img_Material* getMaterialTex(const std::string& name) const;
	unsigned getVolCount() const { return _actorTex3D_map.size(); }
	const Img_Volume* getVolumeTex(const std::string& name) const;
#endif

	// Dynaimc Operations
	void addForce(const std::string& name, const Vec3f& vec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2); // links 2 named geometry actors
	void addAnchor(Phys_Connector* connector, const std::string& name, const Vec3f* pos); // anchors target named geometry object
	void remConnector(const std::string& targetActor); // breaks all connectors associated with named geometry
	void resolvePhysics(); // iterates through all physics objects and applies forces 
private:
	std::vector<const Topl_Light*> _lightSrc; // stores all light sources
#ifdef RASTERON_H
	std::map<Geo_Actor*, const Rasteron_Image*> _actorTex_map; // associates geometry actor to single texture
	std::map<Geo_Actor*, const Img_Material*> _actorTex2D_map; // associates geometry actor to multiple 2D textures
	std::map<Geo_Actor*, const Img_Volume*> _actorTex3D_map; // associates geometry actor to volumetric texture
#endif
	std::vector<Geo_Actor*> _geoActors; // stores all geometries
	std::map<Geo_Actor*, Phys_Actor*> _actorPhys_map; // associates geometry to a physics structure
	std::vector<LinkedItems> _linkedItems; // stores linked actors and connectors
	std::vector<AnchoredItems> _anchoredItems; // stores anchored actors and connectors
	
	Timer_Ticker _ticker; // used for internal updates
};

#define TOPL_SCENE_H
#endif