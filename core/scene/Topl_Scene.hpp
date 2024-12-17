#ifndef TOPL_SCENE_H

#include <map>

#include "support_def.h"

#include "IO.hpp"
#include "Timer.hpp"

#include "Geo_Actor.hpp"
// #include "Geo_Construct.hpp"

#include "images/Img.h"
#include "images/Img_UI.h"

#ifdef TOPL_ENABLE_PHYSICS
#include "physics/Phys_Actor.h"
#include "physics/Phys_Connector.h"
#endif

#include "Topl_Camera.hpp"

// Scene

class Topl_Scene {
public:
	Topl_Scene() { } // Empty Constructor
	Topl_Scene(Topl_Camera* cam){ camera = cam; } // Camera Constuctor
	Topl_Scene(std::initializer_list<Geo_Actor*> actors){ for(auto a = actors.begin(); a != actors.end(); a++) addGeometry(*a); } // Actors Constuctor
	Topl_Scene(Topl_Camera* cam, std::initializer_list<Geo_Actor*> actors){ // Camera dnd Actors Constructor
		camera = cam;
		for(auto a = actors.begin(); a != actors.end(); a++) addGeometry(*a);
	}
	/* Topl_Scene(std::initializer_list<Geo_Construct*> constructs){ // Constructs Constuctor
		for(auto c = constructs.begin(); c != constructs.end(); c++) (*c)->configure(this);
		_ticker.reset();
	} */
	// Topl_Scene(const std::string& filePath) { loadFromFile(filePath); } // File Load Constructor
	~Topl_Scene() {}

	// void saveToFile(const std::string& fileName); // saves scene data to .tp file
	// void loadFromFile(const std::string& filePath); // loads scene data from .tp file

	// Basic Operations

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
	bool getIsTextured() const { return _isTextured; }
#ifdef RASTERON_H
	const Img_Base* getTexture(const std::string& name) const;
	const Img_Sequence* getArrayTex(const std::string& name) const;
	const Img_Volume* getVolumeTex(const std::string& name) const;
#endif
	void removeActor(const std::string& name);

	// Dynaimc Operations
#ifdef TOPL_ENABLE_PHYSICS
	void resolvePhysics(); // iterates through all physics objects and applies forces 
	void addForce(const std::string& name, const Vec3f& vec);
	void addForces(const std::string& name, const Vec3f& posVec, const Vec3f& angleVec, const Vec3f& sizeVec);
	void addPhysics(const std::string& name, Phys_Actor* physActor);
	void addLink(Phys_Connector* connector, const std::string& name1, const std::string& name2); // links 2 named geometry actors
	void addAnchor(Phys_Connector* connector, const std::string& name, const Vec3f* pos); // anchors target named geometry object
	void remConnector(const std::string& targetActor); // breaks all connectors associated with named geometry
#endif
	Topl_Camera* camera = &_defaultCamera;
private:
	std::vector<Geo_Actor*> _geoActors; // stores all geometries
	// std::vector<const Topl_Light*> _lights; // stores all light sources
	std::map<Geo_Actor*, pickerCallback> _pickerCallbackMap;
#ifdef TOPL_ENABLE_PHYSICS
	std::vector<LinkedItems> _linkedItems; // stores linked actors and connectors
	std::vector<AnchoredItems> _anchoredItems; // stores anchored actors and connectors
	std::map<Geo_Actor*, Phys_Actor*> _physicsMap; // associates geometry to a physics structure
#endif
	bool _isTextured = false; // sets to true on texture operations
#ifdef RASTERON_H
	Img_Base _textures[MAX_TEX_BINDINGS] = { 
		Img_Base(std::string(IMAGES_DIR) + "Background-Action.bmp"),
		Img_Base(std::string(IMAGES_DIR) + "Background-Grid.png"), 
		Img_Base(std::string(IMAGES_DIR) + "Background-Pins.png"), 
		Img_Base(std::string(IMAGES_DIR) + "Background-Trigs.png"), 
		Img_Base(std::string(IMAGES_DIR) + "Background-Frame.png"), 
		Img_Base(std::string(IMAGES_DIR) + "Background-Canvas.png"),
		Img_Base(std::string(IMAGES_DIR) + "Background-Squared.png"), 
		Img_Base(std::string(IMAGES_DIR) + "Background-Circular.png"),
	};
	std::map<Geo_Actor*, const Img_Base*> _textureMap; // associates geometry actor to single texture // TODO: Reverse order
	std::map<Geo_Actor*, const Img_Sequence*> _arrayTexMap; // associates geometry actor to multiple 2D textures // TODO: Reverse order
	std::map<Geo_Actor*, const Img_Volume*> _volumeTexMap; // associates geometry actor to volumeTextric texture // TODO: Reverse order
#endif
	Topl_Camera _defaultCamera;
	Timer_Persist _ticker; // used for internal updates
};

typedef const Topl_Scene* const scene_cptr;


#define TOPL_SCENE_H
#endif
