#include "Geometry.hpp"

class Geo_Conic : public Geo_RenderObj {
public:
    // All Shape Constructor
    Geo_Conic(NGon2D refShape, Vec3f apex) 
    : Geo_RenderObj
    (refShape.segments + 2, // Vertex count is number of segments +1 for the center point and +1 for the apex point
     refShape.segments * 6){ // Each segment requires 1 triangle for base and 1 triangle to connect to apex (6 vertices total)
        _shape2D = refShape; // copy to internal data
        _apex = apex;
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
    Vec3f _apex;
};

struct Geo_ConicTriangle : public Geo_Conic { 
    Geo_ConicTriangle(float radius, Vec3f apex) : Geo_Conic({ radius, 3 }, apex){}
};

struct Geo_ConicSquare : public Geo_Conic { // i.e. Pyra_id
    Geo_ConicSquare(float radius, Vec3f apex) : Geo_Conic({ radius, 4 }, apex){}
};

struct Geo_ConicHex : public Geo_Conic {
    Geo_ConicHex(float radius, Vec3f apex) : Geo_Conic({ radius, 6 }, apex){}
};

struct Geo_ConicCircle : public Geo_Conic { // i.e. Cone
    Geo_ConicCircle(float radius, Vec3f apex) : Geo_Conic({ radius, DEFAULT_CIRCLE_SEGS }, apex){}
};

