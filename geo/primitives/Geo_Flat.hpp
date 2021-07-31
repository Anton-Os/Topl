#include "Geometry.hpp"

class Geo_Flat : public Geo_RenderObj {
public:
    // All Shape Constructor
    Geo_Flat(NGon2D refShape) 
    : Geo_RenderObj
    (refShape.segments + 1, // vertex count is number of segments +1 for the center point
     refShape.segments * 3 ){ // each segment requires 1 triangle (3 vertices total)
        mShape2D = refShape; // copy to internal data
        fillRenderObject();
    }

    float getRadius() const { return mShape2D.radius; }
    unsigned getSegments() const { return mShape2D.segments; }
private:
    void genVertices(Eigen::Vector3f* data) override;
    void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
    NGon2D mShape2D;
};

struct Geo_FlatTriangle : public Geo_Flat {
    Geo_FlatTriangle(float radius) : Geo_Flat({ radius, 3 }){}
};

struct Geo_FlatSquare : public Geo_Flat {
    Geo_FlatSquare(float radius) : Geo_Flat({ radius, 4 }){}
};

struct Geo_FlatHex : public Geo_Flat {
    Geo_FlatHex(float radius) : Geo_Flat({ radius, 6 }){}
};

struct Geo_FlatCircle : public Geo_Flat {
    Geo_FlatCircle(float radius) : Geo_Flat({ radius, DEFAULT_CIRCLE_SEGS }){}
};

// Extended Types

#define DEFAULT_PLANE_LENGTH 10.00 // should stretch as far as possible

struct Geo_Plane : public Geo_FlatSquare {
	/* Geo_Plane(Eigen::Vector3f a1) // Orthogonal plane constructor
	: Geo_FlatSquare(DEFAULT_PLANE_LENGTH) {
		axis1 = a1.norm();
		axis2 = Eigen::Vector3f(axis1.z(), axis1.y(), axis1.x());
	} */
	Geo_Plane(Eigen::Vector3f a1, Eigen::Vector3f a2) // Arbitrary plane constructor
		: Geo_FlatSquare(DEFAULT_PLANE_LENGTH) {
		axis1 = a1; 
		axis1.normalize();
		axis2 = a2;
		axis2.normalize();
	}

	Eigen::Vector3f axis1;
	Eigen::Vector3f axis2;
};

