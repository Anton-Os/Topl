#ifndef TOPL_SCENE_H

#include <map>

#include "support_def.h"

#include "IO.hpp"
#include "Timer.hpp"
#include "ValueGen.hpp"

#include "Geo_Actor.hpp"

#include "images/Img.h"
#include "images/Img_UI.h"

#ifdef TOPL_ENABLE_PHYSICS
#include "physics/Phys_Actor.h"
#include "physics/Phys_Connector.h"
#endif

// Camera

#define CAM_ZOOM 1.0f
#define CAM_DEPTH -0.99F

class Topl_Camera {
public:
	Topl_Camera() { // Identity matrix
		_projMatrix = MAT_4x4_IDENTITY;
		_projMatrix(2, 2) = -1; // flip z axis
	}
	Topl_Camera(enum PROJECTION_Type projType){ // Regular Bounds
		_projMatrix = Projection(projType).genProjMatrix();
	}
	Topl_Camera(enum PROJECTION_Type projType, float scaleFactor){ // Sized Bounds
		_projMatrix = Projection(projType, scaleFactor).genProjMatrix();
	}
	Topl_Camera(Projection proj){ // Custom Bounds
		_projMatrix = proj.genProjMatrix();
	}
	void setPos(const Vec3f& pos){ _pos = pos; }
	void updatePos(const Vec3f& vec){ _pos = _pos + vec; }
	void setRot(const Vec3f& vec){ _rotation = vec; }
	void updateRot(const Vec3f& vec) { _rotation = _rotation + vec; }
	void setZoom(float z){ _zoom = z;}
	void setProjMatrix(Mat4x4 matrix){ _projMatrix = matrix; }
	const float* getZoom() const { return &_zoom;}
	vec3f_cptr_t getPos() const { return &_pos; }
	vec3f_cptr_t getRot() const { return &_rotation; }
	mat4x4_cptr_t getProjMatrix() const { return &_projMatrix; }
private:
	float _zoom = CAM_ZOOM; 
	Vec3f _pos = Vec3f({ 0.0f, 0.0f, CAM_DEPTH }); // in front of scene
	Vec3f _rotation = Vec3f({ 0.0f, 0.0f, 0.0f }); // default pointing forward
	Mat4x4 _projMatrix = MAT_4x4_IDENTITY;
};

typedef const Topl_Camera* const camera_cptr;

// Scene

class Topl_Scene {
public:
	Topl_Scene() { _ticker.reset(); } // Empty Constructor
	/* Topl_Scene(const std::string& filePath) { // File Load Constructor
		// loadFromFile(filePath); 
	} */
	~Topl_Scene() {}

	// void saveToFile(const std::string& fileName); // saves scene data to .tp file
	// void loadFromFile(const std::string& filePath); // loads scene data from .tp file

	// Static Operations

	void addGeometry(Geo_Actor* actor); // add geometry
	void addGeometry(const std::string& name, Geo_Actor* actor); // add named geometry
#ifdef RASTERON_H
	void addTexture(const std::string& name, const Img_Base* image);
	void addArrayTex(const std::string& name, const Img_Sequence* arrayTex);
	void addVolumeTex(const std::string& name, const Img_Volume* volumeTex);
#endif
	unsigned getActorCount() const { return _geoActors.size(); }
	Geo_Actor* getPickActor(unsigned color);
	actor_cptr getGeoActor(unsigned index) const; // access to geometry by index
	actor_cptr getGeoActor(const std::string& name) const; // access to geometry by name
	// unsigned getLightCount() const { return _lights.size(); }
	// light_cptr getLight(unsigned index) const; // access to light source by index
#ifdef RASTERON_H
	bool getIsTextured() const { return _isTextured; }
	const Img_Base* getTexture(const std::string& name) const;
	const Img_Sequence* getArrayTex(const std::string& name) const;
	const Img_Volume* getVolumeTex(const std::string& name) const;
#endif
	void removeActor(const std::string& name);

	// Dynaimc Operations
#ifdef TOPL_ENABLE_PHYSICS
	void resolvePhysics(); // iterates through all physics objects and applies forces 
	void addForce(const std::string& name, const Vec3f& vec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2); // links 2 named geometry actors
	void addAnchor(Phys_Connector* connector, const std::string& name, const Vec3f* pos); // anchors target named geometry object
	void remConnector(const std::string& targetActor); // breaks all connectors associated with named geometry
#endif
private:
	std::vector<Geo_Actor*> _geoActors; // stores all geometries
	// std::vector<const Topl_Light*> _lights; // stores all light sources
	std::map<Geo_Actor*, pickerCallback> _pickerCallbackMap;
#ifdef TOPL_ENABLE_PHYSICS
	std::vector<LinkedItems> _linkedItems; // stores linked actors and connectors
	std::vector<AnchoredItems> _anchoredItems; // stores anchored actors and connectors
	std::map<Geo_Actor*, Phys_Actor*> _physicsMap; // associates geometry to a physics structure
#endif
#ifdef RASTERON_H
	bool _isTextured; // sets to true on texture operations
	Img_Base _textures[MAX_TEX_BINDINGS] = { Img_Base(0xAA0000FF), Img_Base(0xAA00FF00), Img_Base(0xAAFF0000), Img_Base(0xAA00FFFF), Img_Base(0xAAFF00FF), Img_Base(0xAAFFFF00), Img_Base(0xAAEEEEEE), Img_Base(0xAA333333) };
	std::map<Geo_Actor*, const Img_Base*> _textureMap; // associates geometry actor to single texture // TODO: Reverse order
	std::map<Geo_Actor*, const Img_Sequence*> _arrayTexMap; // associates geometry actor to multiple 2D textures // TODO: Reverse order
	std::map<Geo_Actor*, const Img_Volume*> _volumeTexMap; // associates geometry actor to volumeTextric texture // TODO: Reverse order
#endif
	Timer_Persist _ticker; // used for internal updates
};

#define TOPL_SCENE_H
#endif