#ifndef GEO_COMPONENT_H

#include "Geometry.hpp"
#include <Eigen/Dense>

class Geo_Component {
public:
	Geo_Component(){
		_id_count++;
		_id = _id_count;
	}
	Geo_Component(const Geo_RenderObj* renderObj) {
		_id_count++;
		_id = _id_count;
		_renderObj = renderObj; 
	}
	~Geo_Component() {
		_id_count--;
	}

	void setName(const std::string& name) { _name = name; }
	void updatePos(Eigen::Vector3f vec){ _worldPos += vec; }
	void updateRot(Eigen::Vector2f angles) { _worldRot += angles; } // TODO: Check for 2 pi boundaries

	unsigned getId() const { return _id; }
	std::string getName() const { return _name; }
	vec3f_cptr getPos() const { return &_worldPos; }
	vec2f_cptr getAngles() const { return &_worldRot; }
	const Geo_RenderObj* getRenderObj() const { return _renderObj; }
	// const Geo_RenderObj* _renderObj;
private:
	// Identification Types
	static unsigned _id_count; // Grows/shrinks when objects are created/deleted
	unsigned _id; // Each object has a unique id
	std::string _name = "_"; // Default is just an underscore

	// Internal Types
	Eigen::Vector3f _worldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector2f _worldRot = Eigen::Vector2f(0.0, 0.0); // Angles by which to rotate
	const Geo_RenderObj* _renderObj;
};

#define GEO_COMPONENT_H
#endif