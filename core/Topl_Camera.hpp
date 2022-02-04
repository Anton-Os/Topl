
#include "Geometry.hpp"
#include "ValueGen.hpp"

class Topl_Camera {
public:
	// Identity projection constructor
	Topl_Camera() { _projMatrix = Eigen::Matrix4f::Identity(); } // Identity matrix by default
	Topl_Camera(enum PROJECTION_Type projType, SpatialBounds3D bounds){
		if (projType == PROJECTION_Perspective) _projMatrix = ValueGen::genPerspectiveMatrix(bounds);
		else if(projType == PROJECTION_Ortho) _projMatrix = ValueGen::genOrthoMatrix(bounds);
	}
	void setPos(const Eigen::Vector3f& pos){ _pos = pos; }
	void movePos(const Eigen::Vector3f& move){ _pos += move; }
	void setLookPos(const Eigen::Vector3f& lookPos){ _lookPos = lookPos; }
	vec3f_cptr getPos() const { return &_pos; }
	vec3f_cptr getLookPos() const { return &_lookPos; }
	mat4f_cptr getProjMatrix() const { return &_projMatrix; }
private:
	Eigen::Vector3f _pos = Eigen::Vector3f(0.0, 0.0, -1.0);
	Eigen::Vector3f _lookPos = Eigen::Vector3f(0.0, 0.0, 0.0);
	Eigen::Matrix4f _projMatrix = Eigen::Matrix4f::Zero();
};

typedef const Topl_Camera* const camera_cptr;