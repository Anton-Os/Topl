#ifndef TOPL_SCENE_H

#include <map>

#include "physics.h"
#include "physics_motion.h"

#include "image_type.h"
#include "support_def.h"

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
	// Identity projection constructor
	Topl_Camera() { _projMatrix = MAT_4x4_IDENTITY; } // Identity matrix
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
	void setLookPos(const Vec3f& vec){ _lookPos = vec; }
	void updateLookPos(const Vec3f& vec){ _lookPos = _pos + vec; }
	vec3f_cptr_t getPos() const { return &_pos; }
	vec3f_cptr_t getLookPos() const { return &_lookPos; }
	mat4x4_cptr_t getProjMatrix() const { return &_projMatrix; }
private:
	Vec3f _pos = Vec3f({ 0.0f, 0.0f, -1.0f });
	Vec3f _lookPos = Vec3f({ 0.0f, 0.0f, 1.0f });
	Mat4x4 _projMatrix = MAT_4x4_IDENTITY;
};

typedef const Topl_Camera* const camera_cptr;

// Scene

class Topl_Scene {
public:
	Topl_Scene() { _ticker.reset(); } // Empty Constructor
	Topl_Scene(const std::string& filePath) { // File Load Constructor
		_ticker.reset();
		// TODO: Add loading logic here
	}
	~Topl_Scene() {}

	// Statics Section
	void addGeometry(Geo_Actor* actor); // add geometry
	void addGeometry(const std::string& name, Geo_Actor* actor); // add geometry and override name
	void addLight(Topl_Light* ls){ _lightSrc.push_back(ls); }
#ifdef RASTERON_H
	void addTexture(const std::string& name, const Rasteron_Image* image);
	void addMaterial(const std::string& name, const Img_Material* material);
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
	const Img_Material* getMaterial(const std::string& name) const;
#endif

	// Dynamics Section
	void addForce(const std::string& name, const Vec3f& vec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2); // links 2 named geometry actors
	void addAnchor(Phys_Connector* connector, const std::string& name, const Vec3f* pos); // anchors target named geometry object
	void remConnector(const std::string& targetActor); // breaks all connectors associated with named geometry
	void resolvePhysics(); // iterates through all physics objects and applies forces 

	void saveToFile(const std::string& fileName); // saves scene data to .tp file
private:
	void loadFromFile(const std::string& filePath); // loads scene data from .tp file

	// Topl_Camera _camera;
	std::vector<Topl_Light*> _lightSrc; // stores all light sources
#ifdef RASTERON_H
	std::map<Geo_Actor*, const Rasteron_Image*> _actorTex_map; // associates geometry actor to a single texture
	// std::map<Geo_Actor*, const Img_Base*> __actorTex_map; // associates geometry actor to a single texture
	std::map<Geo_Actor*, const Img_Material*> _actorMaterial_map; // associates geometry actor to multiple textures
#endif
	std::vector<Geo_Actor*> _geoActors; // stores all geometries
	std::map<Geo_Actor*, Phys_Actor*> _actorPhys_map; // associates geometry to a physics structure
	std::vector<LinkedItems> _linkedItems; // stores linked actors and connectors
	std::vector<AnchoredItems> _anchoredItems; // stores anchored actors and connectors
	
	Timer_Ticker _ticker; // used for internal updates
};

#define TOPL_SCENE_H
#endif