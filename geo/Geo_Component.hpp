#ifndef GEO_COMPONENT_H

#include "Geometry.hpp"
#include <Eigen/Dense>

class Geo_Component {
public:
	Geo_Component(){
		mId_count++;
		mId = mId_count;
	}
	Geo_Component(const Geo_RenderObj* renderObj) {
		mId_count++;
		mId = mId_count;
		mRenderObj = renderObj; 
	}
	~Geo_Component() {
		mId_count--;
	}

	void setName(const std::string& name) { mName = name; }
	void updatePos(Eigen::Vector3f vec){ mRelWorldPos += vec; }
	void updateRot(Eigen::Vector2f angles) { mRotAngles += angles; } // TODO: Check for 2 pi boundaries

	unsigned getId(){ return mId; }
	std::string getName() const { return mName; }
	vec3f_cptr getPos() const { return &mRelWorldPos; }
	vec2f_cptr getAngles() const { return &mRotAngles; }
	const Geo_RenderObj* getRenderObj() const { return mRenderObj; }
	// const Geo_RenderObj* mRenderObj;
private:
	// IDENTIFICATION TYPES
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id
	std::string mName = "_"; // Default is just an underscore

	// INTERNAL DATA TYPES
	Eigen::Vector3f mRelWorldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector2f mRotAngles = Eigen::Vector2f(0.0, 0.0); // Angles by which to rotate
	const Geo_RenderObj* mRenderObj;
};

#define GEO_COMPONENT_H
#endif