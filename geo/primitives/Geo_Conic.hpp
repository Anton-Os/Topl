#include "Geometry.hpp"

class Geo_Conic : public Geo_RenderObj {
public:
    // All Shape Constructor
    Geo_Conic(NGon2D refShape) 
    : Geo_RenderObj
    (refShape.segments + 2, // Vertex count is number of segments +1 for the center point and +1 for the apex point
     refShape.segments * 6 ){ // Each segment requires 1 triangle for base and 1 triangle to connect to apex (6 vertices total)
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

struct Geo_ConicTriangle : public Geo_Conic { 
    Geo_ConicTriangle(float radius) : Geo_Conic({ radius, 3 }){}
};

struct Geo_ConicSquare : public Geo_Conic { // i.e. Pyramid
    Geo_ConicSquare(float radius) : Geo_Conic({ radius, 4 }){}
};

struct Geo_ConicHex : public Geo_Conic {
    Geo_ConicHex(float radius) : Geo_Conic({ radius, 6 }){}
};

struct Geo_ConicCircle : public Geo_Conic { // i.e. Cone
    Geo_ConicCircle(float radius) : Geo_Conic({ radius, DEFAULT_CIRCLE_SEGS }){}
};

