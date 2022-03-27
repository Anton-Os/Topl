#ifndef GEO_ACTOR_H

#include "Geometry.hpp"

#define DEFAULT_ACTOR_NAME "actor"

class Geo_Actor {
public:
	Geo_Actor(){
		_idCount++;
		_id = _idCount;
	}
	Geo_Actor(const Geo_RenderObj* renderObj) {
		_idCount++;
		_id = _idCount;
		_renderObj = renderObj; 
	}
	~Geo_Actor() { _idCount--; }

	void setName(const std::string& name) { _name = name; }
	void setRenderObj(const Geo_RenderObj* renderObj){ _renderObj = renderObj; }

	// void setPos(Eigen::Vector3f vec){ _worldPos = vec; }
	// void updatePos(Eigen::Vector3f vec){ _worldPos += vec; }
	void setPos(const Vec3f& moveVec){ _position = moveVec; }
	void updatePos(const Vec3f& moveVec){ _position = _position + moveVec; }
	
	/* void setRot(Eigen::Vector2f angleVec) { 
		_worldRot = angleVec;
		clampAngles();
	}
	void updateRot(Eigen::Vector2f angleVec) { 
		_worldRot += angleVec;
		clampAngles();
	} */
	void setRot(const Vec2f& angleVec){ _rotation = angleVec; }
	void updateRot(const Vec2f& angleVec){ _rotation = _rotation + angleVec; }
	
	unsigned getId() const { return _id; } // id is immutable
	std::string getName() const { return _name; }

	const Geo_RenderObj* getRenderObj() const { return _renderObj; }
	vec3f_cptr_t getPos() const { return &_position; }
	// vec3f_cptr_t getPos() const { return &_position; }
	// Vec3f getPosition() const { return Vec3f({ _worldPos.x(), _worldPos.y(), _worldPos.z() }); } // change all references of getPos()
	Vec3f getPosition() const { return _position; }
	// vec2f_cptr getRot() const { return &_worldRot; }
	// vec2f_cptr_t getRot() const { return &_rotation; }
	vec2f_cptr_t getRot() const { return &_rotation; }
	Vec2f getRotation() const { return _rotation; }
private:
	/* void clampAngles(){
		while(_worldRot.x() > TOPL_PI * 2.0) _worldRot.x() -= TOPL_PI * 2.0;
		while(_worldRot.x() < TOPL_PI * -2.0) _worldRot.x() += TOPL_PI * 2.0;

		while (_worldRot.y() > TOPL_PI * 2.0) _worldRot.y() -= TOPL_PI * 2.0;
		while (_worldRot.y() < TOPL_PI * -2.0) _worldRot.y() += TOPL_PI * 2.0;
	} */

	// Identification Types
	unsigned _id; // Each object has a unique id
	static unsigned _idCount; // Grows/shrinks when objects are created/deleted
	std::string _name = DEFAULT_ACTOR_NAME;

	// Data Types
	// Eigen::Vector3f _worldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Vec3f _position = Vec3f({ 0.0f, 0.0f, 0.0f });
	// Eigen::Vector2f _worldRot = Eigen::Vector2f(0.0, 0.0); // Angles by which to rotate
	Vec2f _rotation = Vec2f({ 0.0f, 0.0f});
	const Geo_RenderObj* _renderObj = nullptr;
};

#define GEO_ACTOR_H
#endif