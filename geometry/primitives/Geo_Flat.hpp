#ifndef GEO_FLAT_H

#include "Geometry.hpp"

class Geo_Flat : public Geo_RenderObj {
public:
    // Generic Constructor
    Geo_Flat(NGon2D refShape) 
    : Geo_RenderObj
    (refShape.segments + 1, // vertex count is number of segments +1 for the center point
     refShape.segments * 3 ){ // each segment requires 1 triangle (3 vertices total)
        _shape2D = refShape; // copy to internal data
        fillRenderObj();
    }

    // Z Value Constructor
    Geo_Flat(NGon2D refShape, float z) 
    : Geo_RenderObj
    (refShape.segments + 1, // vertex count is number of segments +1 for the center point
     refShape.segments * 3 ){ // each segment requires 1 triangle (3 vertices total)
        _shape2D = refShape; // copy to internal data
        _depth = z;
        fillRenderObj();
    }

    float getRadius() const { return _shape2D.radius; }
    unsigned getSegments() const { return _shape2D.segments; }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;

    NGon2D _shape2D;
    float _depth = DEFAULT_Z;
};

struct Geo_FlatTriangle : public Geo_Flat {
    Geo_FlatTriangle(float radius) : Geo_Flat({ radius, 3 }){}
    Geo_FlatTriangle(float radius, float z) : Geo_Flat({ radius, 3 }, z){}
};

class Geo_FlatSquare : public Geo_Flat {
public:
    Geo_FlatSquare(float radius) : Geo_Flat({ radius, 4 }){}
    Geo_FlatSquare(float radius, float z) : Geo_Flat({ radius, 4 }, z){}
};

struct Geo_FlatHex : public Geo_Flat {
    Geo_FlatHex(float radius) : Geo_Flat({ radius, 6 }){}
    Geo_FlatHex(float radius, float z) : Geo_Flat({ radius, 6 }, z){}
};

struct Geo_FlatCircle : public Geo_Flat {
    Geo_FlatCircle(float radius) : Geo_Flat({ radius, DEFAULT_CIRCLE_SEGS }){}
    Geo_FlatCircle(float radius, float z) : Geo_Flat({ radius, DEFAULT_CIRCLE_SEGS }, z){}
};

// Extended Types

#define DEFAULT_PLANE_LENGTH 10000.00 // should stretch as far as possible

struct Geo_Plane : public Geo_FlatSquare {
	Geo_Plane(Vec3f a1, Vec3f a2) // Arbitrary plane constructor
		: Geo_FlatSquare(DEFAULT_PLANE_LENGTH) {
		axis1 = a1; 
		axis1.normalize();
		axis2 = a2;
		axis2.normalize();
	}

	Vec3f axis1;
	Vec3f axis2;
};

#define GEO_FLAT_H
#endif