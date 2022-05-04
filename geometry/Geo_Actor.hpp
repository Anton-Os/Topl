#ifndef GEO_ACTOR_H

#include "Geometry.hpp"

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
	std::string _name = DEFAULT_ACTOR_NAME;

	// Data Types
	Vec3f _position = Vec3f({ 0.0f, 0.0f, 0.0f });
	Vec2f _rotation = Vec2f({ 0.0f, 0.0f});
	const Geo_RenderObj* _renderObj = nullptr;
};

#define GEO_ACTOR_H
#endif