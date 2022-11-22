#ifndef GEO_ACTOR_H

#include "physics_connector.h"

#include "Geometry.hpp"

// Actor

#define DEFAULT_ACTOR_NAME "actor"

class Geo_Actor {
public:
	Geo_Actor(){}
	Geo_Actor(const Geo_RenderObj* renderObj) { _renderObj = renderObj; }

	void setName(const std::string& name) { _name = name; }
	std::string getName() const { return _name; }
	void setRenderObj(const Geo_RenderObj* renderObj){ _renderObj = renderObj; }
	const Geo_RenderObj* getRenderObj() const { return _renderObj; }

	void setPos(const Vec3f& moveVec){ _position = moveVec; }
	void updatePos(const Vec3f& moveVec){ _position = _position + moveVec; }
	void setRot(const Vec3f& angleVec){ _rotation = angleVec; }
	void updateRot(const Vec3f& angleVec){ _rotation = _rotation + angleVec; }

	vec3f_cptr_t getPos() const { return &_position; }
	Vec3f getPosition() const { return _position; }
	vec3f_cptr_t getRot() const { return &_rotation; }
	Vec3f getRotation() const { return _rotation; }
private:
	// Data Types
	std::string _name = DEFAULT_ACTOR_NAME;
	const Geo_RenderObj* _renderObj = nullptr;
	const Geo_InstancedObj* _instancedObj = nullptr;

	// Orientation Types
	Vec3f _position = Vec3f({ 0.0f, 0.0f, 0.0f });
	Vec3f _rotation = Vec3f({ 0.0f, 0.0f});
};

typedef const Geo_Actor* const actor_cptr;

// Links and Anchors

typedef std::pair<const Geo_Actor*, const Geo_Actor*> link_pair;
typedef std::pair<const Geo_Actor*, const Vec3f*> anchor_pair;

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

#define GEO_ACTOR_H
#endif