#ifndef GEO_VERTEX_H

#include "support_def.h"

#include "maths.h"

// Vertices

enum VERTEX_Attrib { VERTEX_Pos = 0, VERTEX_Normal = 1, VERTEX_Texcoord = 2 };

enum AXIS_Target { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 };

struct Geo_Vertex {
	Geo_Vertex() {} // empty constructor
    Geo_Vertex(Vec3f p){ // position constructor
		position = p;
        texcoord = getTexCoord(position);
    }
	Geo_Vertex(Vec3f p, /* Vec3f n, */ Vec3f t) { // full constructor
		position = p; 
		/* normal = n; */ 
		texcoord = t;
	}

    Vec3f getTexCoord(Vec3f vertex){ // regular texture coordinates
        Vec3f normVec = vertex;
        normVec.normalize();
        return Vec3f({ (normVec[0] + 1.0F) / 2.0F, 1.0F - ((normVec[1] + 1.0F) / 2.0F), (normVec[2] + 1.0F) / 2.0F });
    }

    Vec3f getTexCoord(unsigned v, float z) { // square texture coordinates
        float x = ((v - 1) % 4 == 0 || (v - 1) % 4 == 1)? 1.0f : 0.0f;
        float y = ((v - 1) % 4 == 1 || (v - 1) % 4 == 2) ? 1.0f : 0.0f;
        return Vec3f({ x, y, z });
    }

	Vec3f position = Vec3f({ 0.0, 0.0, 0.0 });
	// Vec3f normal = Vec3f({ 0.0, 0.0, 0.0 });
	Vec3f texcoord = Vec3f({ 0.0, 0.0, 0.0 });
};

typedef const Geo_Vertex* const vertex_cptr_t;

// Transform Callbacks

typedef Vec3f (*vTformCallback)(Vec3f, Vec3f);

static Vec3f shiftTForm(Vec3f input, Vec3f amount) { return Vec3f({input + amount}); } // shifts vertex by amount
static Vec3f rotateTForm(Vec3f input, Vec3f degrees){
    Mat3x3 zRotMatrix = Mat3x3({ cos(degrees[0]), sin(degrees[0]), 0, -sin(degrees[0]), cos(degrees[0]), 0, 0, 0, 1.0F });
    Mat3x3 xRotMatrix = Mat3x3({ 1.0F, 0, 0, 0, cos(degrees[1]), sin(degrees[1]), 0, -sin(degrees[1]), cos(degrees[1]) });
    Mat3x3 yRotMatrix = Mat3x3({ cos(degrees[2]), 0, sin(degrees[2]), 0, 1.0F, 0, -1.0F * sin(degrees[2]), 0, cos(degrees[2]) });
    return zRotMatrix * yRotMatrix * xRotMatrix * input;
}
static Vec3f scaleTForm(Vec3f input, Vec3f factor) { return Vec3f({input * factor}); } // scales vertex by factor

#define GEO_VERTEX_H
#endif
