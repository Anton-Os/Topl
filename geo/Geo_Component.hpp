#include "Geometry.hpp"
#include <Eigen/Dense>

class Geo_Component {
public:
	Geo_Component(){
		mId_count++;
		mId = mId_count;
	}
	Geo_Component(const Geo_RenderObj* renderObj){
		mId_count++;
		mId = mId_count;
		mRenderObj = renderObj; 
	}
	~Geo_Component() {
		mId_count--;
	}

	void setName(const std::string& name) { mName = '_' + name; }
	void updatePos(Eigen::Vector3f vec); // Follow by more spatial update things

	unsigned getId(){ return mId; }
	vec3f_cptr getPos() const { return &mRelWorldPos; }

	const Geo_RenderObj* mRenderObj; // Could be a private member
protected:
	enum GeoBehavior {
		GEO_Fixed = 0, // Should be the default
		GEO_Dynamic = 1
	} mBehavior;
private:
	// IDENTIFICATION TYPES
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id
	std::string mName = "_"; // Default is just an underscore

	// INTERNAL DATA TYPES
	Eigen::Vector3f mRelWorldPos = Eigen::Vector3f(0.0, 0.0, 0.0); // Positions by which to offset
	Eigen::Vector3f mOrientAngl = Eigen::Vector3f(0.0, 0.0, 0.0); // Angles by which to rotate
};