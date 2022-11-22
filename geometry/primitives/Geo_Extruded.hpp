#include "Geometry.hpp"

class Geo_Extruded : public Geo_RenderObj {
public:
    Geo_Extruded(NGon2D refShape) // Generic Constructor
    : Geo_RenderObj
        ((refShape.segments + 1) * 2, // center point on front and back + segments
        refShape.segments * 12 ){ // triangle for front and back and 2 for sides
        _shape2D = refShape;
        _depth = _shape2D.radius;
        init();
    }

    Geo_Extruded(NGon2D refShape, float depth) // Depth Constructor
    : Geo_RenderObj
        ((refShape.segments + 1) * 2, // center point on front and back + segments
        refShape.segments * 12 ){ // triangle for front and back and 2 for sides
        _shape2D = refShape;
        _depth = depth;
        init();
    }

    float getRadius() const { return _shape2D.radius; }
    unsigned getSegments() const { return _shape2D.segments; }
    float getDepth() const { return _depth; }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;
    NGon2D _shape2D;
    float _depth;
};

struct Geo_ExtrudedTriangle : public Geo_Extruded { 
    Geo_ExtrudedTriangle(float radius) : Geo_Extruded({ radius, 3 }){}
    Geo_ExtrudedTriangle(float radius, float depth) : Geo_Extruded({ radius, 3 }, depth){}
};

struct Geo_ExtrudedSquare : public Geo_Extruded { // Box
    Geo_ExtrudedSquare(float radius) : Geo_Extruded({ radius, 4 }){}
    Geo_ExtrudedSquare(float radius, float depth) : Geo_Extruded({ radius, 4 }, depth){}
};

struct Geo_ExtrudedHex : public Geo_Extruded {
    Geo_ExtrudedHex(float radius) : Geo_Extruded({ radius, 6 }){}
    Geo_ExtrudedHex(float radius, float depth) : Geo_Extruded({ radius, 6 }, depth){}
};

struct Geo_ExtrudedCircle : public Geo_Extruded { // Cylinder
    Geo_ExtrudedCircle(float radius) : Geo_Extruded({ radius, DEFAULT_CIRCLE_SEGS }){}
    Geo_ExtrudedCircle(float radius, float depth) : Geo_Extruded({ radius, DEFAULT_CIRCLE_SEGS }, depth){}
};

