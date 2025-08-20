#ifndef GEO_VERTEX_H

#define IGNORE_RASTER
#include "support_def.h"

#include "maths/Vector.h"
#include "maths/Matrix.h"

// Vertices

enum VERTEX_Attrib { VERTEX_Pos = 0, VERTEX_Normal = 1, VERTEX_Texcoord = 2 };

enum AXIS_Target { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 };

struct Geo_Pos {
    Geo_Pos() {}
    Geo_Pos(Vec3f p){ position = p; } // position constructor

    Vec3f position = Vec3f({ 0.0, 0.0, 0.0 });
};

struct Geo_Vertex : public Geo_Pos {
	DllExport Geo_Vertex() : Geo_Pos() {} // empty constructor
    DllExport Geo_Vertex(Vec3f p); // position constructor
    DllExport Geo_Vertex(Vec3f p, Vec3f t, Vec3f n, Vec3f c);
    DllExport Geo_Vertex(const Geo_Vertex& vertex);

    Vec3f getNormal(Vec3f vertexPos);
    Vec3f getTexCoord(Vec3f vertexPos);
    Vec3f getTexCoord(unsigned v, float z);
    Vec3f getColor();
    Vec3f getTangent(Vec3f vertexPos);

    // unsigned segment = 0; // gets recorded when mesh is composite
	Vec3f texcoord = Vec3f({ 0.5F, 0.5F, 0.0 }); // indicates texture mapping
    Vec3f normal = Vec3f({ 0.0, 0.0, -1.0 }); // indicates face direction
    Vec3f color = VEC_3F_ONES; // indicates vertex number
    Vec3f tangent = VEC_3F_ZERO; // gets modified based on the previous vertex
};

typedef const Geo_Vertex* const vertex_cptr_t;
typedef const unsigned* const ui_cptr_t;

// Transform Callbacks

typedef Vec3f (*vTformCallback)(Vec3f, Vec3f);
typedef Vec3f (*iTformCallback)(unsigned, Vec3f, Vec3f);

typedef Geo_Vertex (*vertexTransform)(const Geo_Vertex&, unsigned, unsigned);
typedef Geo_Vertex (*originTransform)(const Geo_Vertex&, const Geo_Vertex&, unsigned, unsigned);

static Vec3f shiftTForm(Vec3f input, Vec3f amount) { return Vec3f({input + amount}); } // shifts vertex by amount
static Vec3f rotateTForm(Vec3f input, Vec3f degrees){
    Mat3x3 zRotMatrix = Mat3x3({ cos(degrees[0]), sin(degrees[0]), 0, -sin(degrees[0]), cos(degrees[0]), 0, 0, 0, 1.0F });
    Mat3x3 xRotMatrix = Mat3x3({ 1.0F, 0, 0, 0, cos(degrees[1]), sin(degrees[1]), 0, -sin(degrees[1]), cos(degrees[1]) });
    Mat3x3 yRotMatrix = Mat3x3({ cos(degrees[2]), 0, sin(degrees[2]), 0, 1.0F, 0, -1.0F * sin(degrees[2]), 0, cos(degrees[2]) });
    return zRotMatrix * yRotMatrix * xRotMatrix * input;
}
static Vec3f scaleTForm(Vec3f input, Vec3f factor) { return Vec3f({input * factor}); } // scales vertex by factor

DllExport Geo_Vertex getMidpoint(std::initializer_list<Geo_Vertex> points);

#define GEO_VERTEX_H
#endif
