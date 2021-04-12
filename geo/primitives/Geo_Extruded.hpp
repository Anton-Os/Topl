#include "Geometry.hpp"

class Geo_Extruded : public Geo_RenderObj {
public:
    // All Shape Constructor
    Geo_Extruded(NGon2D refShape) 
    : Geo_RenderObj
    ((refShape.segments + 1) * 2, // Vertex count is number of segments +1 for the center point but DOUBLED for the back face as well
     refShape.segments * 12){ // Each segment requires 2 triangles for front and back face and 2 triangles for the side face (12 vertices total)
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

struct Geo_ExtrudedTriangle : public Geo_Extruded { 
    Geo_ExtrudedTriangle(float radius) : Geo_Extruded({ radius, 3 }){}
};

struct Geo_ExtrudedSquare : public Geo_Extruded { // i.e. Pyramid
    Geo_ExtrudedSquare(float radius) : Geo_Extruded({ radius, 4 }){}
};

struct Geo_ExtrudedHex : public Geo_Extruded {
    Geo_ExtrudedHex(float radius) : Geo_Extruded({ radius, 6 }){}
};

struct Geo_ExtrudedCircle : public Geo_Extruded { // i.e. Cone
    Geo_ExtrudedCircle(float radius) : Geo_Extruded({ radius, DEFAULT_CIRCLE_SEGS }){}
};

