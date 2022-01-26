#ifndef GEO_COMPONENT_H

#include "Geometry.hpp"
// #include "Physics.hpp"

class Geo_Actor {
public:
	Geo_Actor(){
		_id_count++;
		_id = _id_count;
	}
	Geo_Actor(const Geo_RenderObj* renderObj) {
		_id_count++;
		_id = _id_count;
		_renderObj = renderObj; 
	}
	~Geo_Actor() {
		_id_count--;
	}

	void setName(const std::string& name) { _name = name; }
	void setRenderObj(const Geo_RenderObj* renderObj){ _renderObj = renderObj; }

	void setPos(Eigen::Vector3f vec){ _worldPos = vec; }
	void updatePos(Eigen::Vector3f vec){ _worldPos += vec; }
	void setRot(Eigen::Vector2f angles) { 
		_worldRot = angles;
		clampAngles();
	}
	void updateRot(Eigen::Vector2f angles) { 
		_worldRot += angles;
		clampAngles();
	}
	
	unsigned getId() const { return _id; } // id is immutable
	std::string getName() const { return _name; }
	const Geo_RenderObj* getRenderObj() const { return _renderObj; }
	vec3f_cptr getPos() const { return &_worldPos; }
	vec2f_cptr getAngles() const { return &_worldRot; }
private:
	void clampAngles(){
		while(_worldRot.x() > TOPL_PI * 2.0) _worldRot.x() -= TOPL_PI * 2.0;
		while(_worldRot.x() < TOPL_PI * -2.0) _worldRot.x() += TOPL_PI * 2.0;

		while (_worldRot.y() > TOPL_PI * 2.0) _worldRot.y() -= TOPL_PI * 2.0;
		while (_worldRot.y() < TOPL_PI * -2.0) _worldRot.y() += TOPL_PI * 2.0;
	}

	// Identification Types
	static unsigned _id_count; // Grows/shrinks when objects are created/deleted
	unsigned _id; // Each object has a unique id
	std::string _name = "_"; // Default is just an underscore

	// Internal Types
	Eigen::Vector3f _worldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector2f _worldRot = Eigen::Vector2f(0.0, 0.0); // Angles by which to rotate
	const Geo_RenderObj* _renderObj = nullptr;
};

#define GEO_COMPONENT_H
#endif