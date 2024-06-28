#ifndef GEO_VERTEX_H

#include "support_def.h"

#include "maths.h"

// Vertices

enum VERTEX_Attrib { VERTEX_Pos = 0, VERTEX_Normal = 1, VERTEX_Texcoord = 2 };

enum AXIS_Target { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 };

struct Geo_Vertex {
	Geo_Vertex() {} // empty constructor
	Geo_Vertex(Vec3f p){ 
		position = p;

		double length = (p[0] > p[1])? p[0] : p[1]; // sqrt(pow(p[0], 2) + pow(p[1], 2));
		texcoord = Vec3f({ (float)((p[0] / length) + 1.0F) * 0.5F, (float)((p[1] / length) + 1.0F) * 0.5F, 0.0F });
	} // position constructor
	Geo_Vertex(Vec3f p, /* Vec3f n, */ Vec3f t) { // full constructor
		position = p; 
		/* normal = n; */ 
		texcoord = t;
	}

	Vec3f position = Vec3f({ 0.0, 0.0, 0.0 });
	// Vec3f normal = Vec3f({ 0.0, 0.0, 0.0 });
	Vec3f texcoord = Vec3f({ 0.0, 0.0, 0.0 });
};

typedef const Geo_Vertex* const vertex_cptr_t;

// Transform Callbacks

typedef Vec3f (*vTformCallback)(Vec3f, Vec3f);

static Vec3f shiftTForm(Vec3f input, Vec3f amount) { return Vec3f({input + amount}); } // shifts vertex by amount
// static Vec3f rotateTForm(Vec3f input, Vec3f degrees){ return Vec3f({ 0.0f, 0.0f, 0.0f }); } // TODO: rotates angles by degrees
static Vec3f scaleTForm(Vec3f input, Vec3f factor) { return Vec3f({input * factor}); } // scales vertex by factor

#define GEO_VERTEX_H
#endif