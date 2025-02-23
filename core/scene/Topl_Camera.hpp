#include "maths/Vector.h"
#include "maths/Matrix.h"

// Projection

#define PPROJ_A -1.0
#define PPROJ_Z 5.0

enum PROJECTION_Type {
    PROJECTION_None,
    PROJECTION_Orthographic,
    PROJECTION_Perspective,
    PROJECTION_Hyperbolic,
    PROJECTION_Experimental,
};

struct Projection { // Used in Matrix calculations
    Projection(){}
    Projection(PROJECTION_Type p){ type = p; }
    Projection(PROJECTION_Type p, float s){
        type = p;
        left *= s; right *= s;
        bottom *= s; top *= s;
        nearPlane *= s; farPlane *= s;
    }
    Projection(PROJECTION_Type p, float l, float r, float b, float t, float n, float f){
        type = p;
        left *= l; right *= r;
        bottom *= b; top *= t;
        nearPlane *= n; farPlane *= f;
    }

    Mat4x4 genProjMatrix(){
        switch(type){
            case PROJECTION_Orthographic: return genOrthoMatrix();
            case PROJECTION_Perspective: return genPerspectiveMatrix();
            case PROJECTION_Experimental: return genExperimentalMatrix();
            default: return MAT_4x4_IDENTITY;
        }
    }

    PROJECTION_Type type = PROJECTION_None;
    float left = -1.0f; float right = 1.0f;
    float bottom = -1.0f; float top = 1.0f;
    float nearPlane = -1.0f; float farPlane = 1.0f;
protected:
    Mat4x4 genOrthoMatrix(){
        float n = nearPlane * PPROJ_Z; 
        float f = farPlane * PPROJ_Z;
            
        return Mat4x4({ // From OpenGL SuperBible starting page 89
            2.0f / (right - left), 0.0f, 0.0f, (left + right) / (left - right),
            0.0f, 2.0f / (top - bottom), 0.0f, (bottom + top) / (bottom - top),
            0.0f, 0.0f, 2.0f / (n - f), (f + n) / (f - n),
            0.0f, 0.0f, 0.0f, 1.0f 
        });
    }

    Mat4x4 genPerspectiveMatrix(){
        float l = left / (PPROJ_A * -1.0); float r = right / (PPROJ_A * -1.0);
        float b = bottom / (PPROJ_A * -1.0); float t = top / (PPROJ_A * -1.0);
        float n = nearPlane / (PPROJ_A / (PPROJ_Z * 0.5F)); 
        float f = farPlane / (PPROJ_A / (PPROJ_Z * 0.5F)); 

        return Mat4x4({ // From OpenGL SuperBible starting page 88
            (2.0f * n) / (r - l), 0.0f, (r + l) / (r - l), 0.0f,
            0.0f, (2.0f * n) / (t - b), (t + b) / (t - b), 0.0f,
            0.0f, 0.0f, -((f + n) / (f - n)), -((2.0f * n * f) / (farPlane - n)),
            0.0f, 0.0f, -1.0f, 0.0f
        });
    }

    Mat4x4 genExperimentalMatrix(){
        return Mat4x4({
            (nearPlane - farPlane) / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, (nearPlane - farPlane) / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, farPlane - nearPlane,  0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        });
    }
};

// Camera

#define CAM_ZOOM 1.0f
#define CAM_DEPTH -0.99F

class Topl_Camera {
public:
	Topl_Camera() { // Identity matrix
		_projMatrix = MAT_4x4_IDENTITY;
		_projMatrix.data[2][2] = -1; // flip z axis
	}
	Topl_Camera(enum PROJECTION_Type projType){ // Regular Bounds
		_projMatrix = Projection(projType).genProjMatrix();
	}
	Topl_Camera(enum PROJECTION_Type projType, float scaleFactor){ // Sized Bounds
		_projMatrix = Projection(projType, scaleFactor).genProjMatrix();
	}
	Topl_Camera(Projection proj){ // Custom Bounds
		_projMatrix = proj.genProjMatrix();
	}
	void setPos(const Vec3f& pos){ _pos = pos; }
	void updatePos(const Vec3f& vec){ _pos = _pos + vec; }
	void setRot(const Vec3f& vec){ _rotation = vec; }
	void updateRot(const Vec3f& vec) { _rotation = _rotation + vec; }
	void setZoom(float z){ _zoom = z;}
	void setProjMatrix(Mat4x4 matrix){ _projMatrix = matrix; }
	const float* getZoom() const { return &_zoom;}
	vec3f_cptr_t getPos() const { return &_pos; }
	vec3f_cptr_t getRot() const { return &_rotation; }
	mat4x4_cptr_t getProjMatrix() const { return &_projMatrix; }
private:
	float _zoom = CAM_ZOOM; 
	Vec3f _pos = Vec3f({ 0.0f, 0.0f, CAM_DEPTH }); // in front of scene
	Vec3f _rotation = Vec3f({ 0.0f, 0.0f, 0.0f }); // default pointing forward
	Mat4x4 _projMatrix = MAT_4x4_IDENTITY;
};

typedef const Topl_Camera* const camera_cptr;