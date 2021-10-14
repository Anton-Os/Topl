
#include "Geometry.hpp"
#include "ValueGen.hpp"

class Topl_Camera {
public:
	// Identity projection constructor
	Topl_Camera() { projMatrix = Eigen::Matrix4f::Identity(); } // Identity matrix by default
	Topl_Camera(enum PROJECTION_Type projType, SpatialBounds3D bounds){
		if (projType == PROJECTION_Perspective) projMatrix = ValueGen::genspectiveMatrix(bounds);
		else if(projType == PROJECTION_Ortho) projMatrix = ValueGen::genOrthoMatrix(bounds);
	}
	void movePos(const Eigen::Vector3f& moveVec){ pos += moveVec; }
	vec3f_cptr getPos() const { return &pos; }
	vec3f_cptr getDirection() const { return &direction; }
	mat4f_cptr getProjMatrix() const { return &projMatrix; }
private:
	Eigen::Vector3f pos = Eigen::Vector3f(0.0, 0.0, -1.0);
	Eigen::Vector3f direction = Eigen::Vector3f(0.0, 0.0, 0.0);
	Eigen::Matrix4f projMatrix = Eigen::Matrix4f::Zero();
};

typedef const Topl_Camera* const topl_camera_cptr;