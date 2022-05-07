#include "Geometry.hpp"

class Geo_Conic : public Geo_RenderObj {
public:
    Geo_Conic(NGon2D refShape) // Generic Constructor
    : Geo_RenderObj
    (refShape.segments + 2, // segments +1 for apex and +1 for center
     refShape.segments * 6){ // 1 base triangle and 1 apex connecting triangle
        _shape2D = refShape;
        _apex = Vec3f({ 0.0f, 0.0f, -1.0f * refShape.radius }); // default apex
        fillRenderObj();
    }

    Geo_Conic(NGon2D refShape, Vec3f apex) // Apex Constructor
    : Geo_RenderObj
    (refShape.segments + 2,  // segments +1 for apex and +1 for center
     refShape.segments * 6){ // 1 base triangle and 1 apex connecting triangle
        _shape2D = refShape;
        _apex = apex; // manual apex
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
    Geo_ConicTriangle(float radius) : Geo_Conic({ radius, 3 }){}
    Geo_ConicTriangle(float radius, Vec3f apex) : Geo_Conic({ radius, 3 }, apex){}
};

struct Geo_ConicSquare : public Geo_Conic { // Pyramid
Geo_ConicSquare(float radius) : Geo_Conic({ radius, 4 }){}
    Geo_ConicSquare(float radius, Vec3f apex) : Geo_Conic({ radius, 4 }, apex){}
};

struct Geo_ConicHex : public Geo_Conic {
    Geo_ConicHex(float radius) : Geo_Conic({ radius, 6 }){}
    Geo_ConicHex(float radius, Vec3f apex) : Geo_Conic({ radius, 6 }, apex){}
};

struct Geo_ConicCircle : public Geo_Conic { // Cone
    Geo_ConicCircle(float radius) : Geo_Conic({ radius, DEFAULT_CIRCLE_SEGS }){}
    Geo_ConicCircle(float radius, Vec3f apex) : Geo_Conic({ radius, DEFAULT_CIRCLE_SEGS }, apex){}
};

