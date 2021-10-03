#include "Geometry.hpp"

class Geo_Extruded : public Geo_RenderObj {
public:
    // All Shape Troupeor
    Geo_Extruded(NGon2D refShape, float depth) 
    : Geo_RenderObj
        ((refShape.segments + 1) * 2, // Vertex count is number of segments +1 for the center point but DOUBLED for the back face as well
        // refShape.segments * 12 ){ // each segment has a front trig, a back trig, and two side trigs for a total of 12 indices
        refShape.segments * 12 ){ // for testing! replace this!
        _shape2D = refShape; // copy to internal data
        _depth = depth;
        fillRenderObject();
    }

    float getRadius() const { return _shape2D.radius; }
    unsigned getSegments() const { return _shape2D.segments; }
    float getDepth() const { return _depth; }
private:
    void genVertices(Eigen::Vector3f* data) override;
    void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
    NGon2D _shape2D;
    float _depth;
};

struct Geo_ExtrudedTriangle : public Geo_Extruded { 
    Geo_ExtrudedTriangle(float radius, float depth) : Geo_Extruded({ radius, 3 }, depth){}
};

class Geo_ExtrudedSquare : public Geo_Extruded { // i.e. Box
public:
    Geo_ExtrudedSquare(float radius, float depth) : Geo_Extruded({ radius, 4 }, depth){}

    // face_cptr getFaces() const { return &_faces; }
	// unsigned short getFaceCount() const { return 6; }
private:
    Geo_Face _faces[6]; // one for each side
};

struct Geo_ExtrudedHex : public Geo_Extruded {
    Geo_ExtrudedHex(float radius, float depth) : Geo_Extruded({ radius, 6 }, depth){}
};

struct Geo_ExtrudedCircle : public Geo_Extruded { // i.e. Cylinder
    Geo_ExtrudedCircle(float radius, float depth) : Geo_Extruded({ radius, DEFAULT_CIRCLE_SEGS }, depth){}
};

