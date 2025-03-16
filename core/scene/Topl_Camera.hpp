#include "maths/Vector.h"
#include "maths/Matrix.h"

// Projection

#define PPROJ_A 1.0F
#define PPROJ_Z 5.0F

#define CAM_ZOOM 1.0f
#define CAM_DEPTH -0.99999F

enum PROJECTION_Type {
    PROJECTION_None,
    PROJECTION_Orthographic,
    PROJECTION_Perspective,
    PROJECTION_Hyperspace,
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

    Mat4x4 genProjMatrix(){ return genProjMatrix(Vec3f({ 0.0F, 0.0F, 0.0F })); }

    Mat4x4 genProjMatrix(Vec3f camPos){
        switch(type){
            case PROJECTION_Orthographic: return genOrthoMatrix();
            case PROJECTION_Perspective: return genPerspectiveMatrix();
            case PROJECTION_Hyperspace: return genHyperspaceMatrix(camPos);
            default: // Identity Matrix with scaling
                Mat4x4 matrix = MAT_4x4_IDENTITY;
                matrix.data[2][2] *= -1.0F; // flip z axis
                return matrix;
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
        float l = left / -PPROJ_A; float r = right / -PPROJ_A;
        float b = bottom / -PPROJ_A; float t = top / -PPROJ_A;
        float n = nearPlane; // * -1.0F; 
        float f = farPlane; // * -1.0F; 

        return Mat4x4({ // From OpenGL SuperBible starting page 88
            (2.0f * n) / (r - l), 0.0f, (r + l) / (r - l), 0.0f,
            0.0f, (2.0f * n) / (t - b), (t + b) / (t - b), 0.0f,
            0.0f, 0.0f, -((f + n) / (f - n)), ((2.0f * n * f) / (f - n)) * (1.0F / PPROJ_Z),
            0.0f, 0.0f, 1.0f, 0.0f
        });
    }

    Mat4x4 genHyperspaceMatrix(Vec3f camPos){
        Mat4x4 matrix = Mat4x4::scale(Vec3f({ 
            (fabs(left) + fabs(right)) / 2.0F, 
            (fabs(top) + fabs(bottom)) / 2.0F, 
            (fabs(nearPlane) + fabs(farPlane)) / 2.0F }
        ));

        matrix.data[2][2] *= -1.0F; // flip z axis
        matrix.data[3][0] = camPos[0];
        matrix.data[3][1] = camPos[1];
        matrix.data[3][2] = camPos[2] - CAM_DEPTH;
        std::cout << "Generating hyperspace matrix from " << camPos.toString() << std::endl;
        return matrix;
    }
};

// Camera

class Topl_Camera {
public:
	Topl_Camera() { // Identity matrix
		_projMatrix = MAT_4x4_IDENTITY;
		_projMatrix.data[2][2] = -1; // flip z axis
	}
	Topl_Camera(enum PROJECTION_Type projType){ // Regular Bounds
		_projMatrix = Projection(projType).genProjMatrix(_pos);
	}
	Topl_Camera(enum PROJECTION_Type projType, float scaleFactor){ // Sized Bounds
		_projMatrix = Projection(projType, scaleFactor).genProjMatrix(_pos);
	}
	Topl_Camera(Projection proj){ // Custom Bounds
		_projMatrix = proj.genProjMatrix(_pos);
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
    // Projection _projection = Projection(PROJECTION_None);
};

typedef const Topl_Camera* const camera_cptr;