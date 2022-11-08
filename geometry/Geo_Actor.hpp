#ifndef GEO_ACTOR_H

#include "physics_connector.h"

#include "Geometry.hpp"

// Actor

#define DEFAULT_ACTOR_NAME "actor"

class Geo_Actor {
public:
	Geo_Actor(){}
	Geo_Actor(const Geo_RenderObj* renderObj) { _renderObj = renderObj; }
	~Geo_Actor(){ }

	void setName(const std::string& name) { _name = name; }
	void setRenderObj(const Geo_RenderObj* renderObj){ _renderObj = renderObj; }

	void setPos(const Vec3f& moveVec){ _position = moveVec; }
	void updatePos(const Vec3f& moveVec){ _position = _position + moveVec; }
	void setRot(const Vec2f& angleVec){ _rotation = angleVec; }
	void updateRot(const Vec2f& angleVec){ _rotation = _rotation + angleVec; }
	
	std::string getName() const { return _name; }

	const Geo_RenderObj* getRenderObj() const { return _renderObj; }
	vec3f_cptr_t getPos() const { return &_position; }
	Vec3f getPosition() const { return _position; }
	vec2f_cptr_t getRot() const { return &_rotation; }
	Vec2f getRotation() const { return _rotation; }
private:
	// unsigned renderID;
	std::string _name = DEFAULT_ACTOR_NAME;

	// Data Types
	Vec3f _position = Vec3f({ 0.0f, 0.0f, 0.0f });
	Vec2f _rotation = Vec2f({ 0.0f, 0.0f});
	const Geo_RenderObj* _renderObj = nullptr;
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