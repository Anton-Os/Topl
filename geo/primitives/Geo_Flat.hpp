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

