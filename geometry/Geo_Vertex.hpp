#ifndef GEO_VERTEX_H

#include "support_def.h"

#include "maths/Vector.h"
#include "maths/Matrix.h"

// Vertices

enum VERTEX_Attrib { VERTEX_Pos = 0, VERTEX_Normal = 1, VERTEX_Texcoord = 2 };

enum AXIS_Target { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 };

struct Geo_Pos {
    Geo_Pos() {}
    Geo_Pos(Vec3f p){ // position constructor
		position = p;
    }

    Vec3f position = Vec3f({ 0.0, 0.0, 0.0 });
};

struct Geo_Vertex : public Geo_Pos {
	Geo_Vertex() : Geo_Pos() {} // empty constructor
    Geo_Vertex(Vec3f p) : Geo_Pos(p){ // position constructor
        normal = p;
        texcoord = getTexCoord(position);
        color = getColor();
    }
    Geo_Vertex(Vec3f p, Vec3f t, Vec3f n, Vec3f c) : Geo_Pos(p) { // full constructor
        texcoord = t;
        normal = n;
        color = c;
    }
    Geo_Vertex(const Geo_Vertex& vertex) : Geo_Pos(vertex.position) { // copy constructor
        texcoord = vertex.texcoord;
        normal = vertex.normal;
        color = vertex.color;
    }

    Vec3f getTexCoord(Vec3f vertexPos){ // regular texture coordinates
        Vec3f normVec = vertexPos;
        normVec.normalize();
        return Vec3f({ (normVec[0] + 1.0F) / 2.0F, 1.0F - ((normVec[1] + 1.0F) / 2.0F), (normVec[2] + 1.0F) / 2.0F });
    }

    Vec3f getTexCoord(unsigned v, float z) { // square texture coordinates
        float x = ((v - 1) % 4 == 0 || (v - 1) % 4 == 1)? 1.0f : 0.0f;
        float y = ((v - 1) % 4 == 1 || (v - 1) % 4 == 2) ? 1.0f : 0.0f;
        return Vec3f({ x, y, z });
    }

    Vec3f getColor(){
        static unsigned vertCount = 0;
        float attenuation = floor(vertCount / 6.0F) * (1.0F / 256.0F);

        if(position.data[0] == 0.0F && position.data[1] == 0.0F && position.data[2] == 0.0F) vertCount = 0; // reset
        else vertCount++; // increment

        if(vertCount % 6 == 0) return Vec3f({ 1.0F - attenuation, 0.0, 0.0 }); // red
        else if (vertCount % 6 == 1) return Vec3f({ 0.0, 1.0F - attenuation, 0.0 }); // green
        else if (vertCount % 6 == 2) return Vec3f({ 0.0, 0.0, 1.0F - attenuation }); // blue
        else if (vertCount % 6 == 3) return Vec3f({ 1.0F - attenuation, 1.0F - attenuation, 0.0 }); // yellow
        else if (vertCount % 6 == 4) return Vec3f({ 0.0, 1.0F - attenuation, 1.0F - attenuation }); // cyan
        else if (vertCount % 6 == 5) return Vec3f({ 1.0F - attenuation, 0.0, 1.0F - attenuation }); // magenta
        else return Vec3f({ 1.0F - attenuation, 1.0F - attenuation, 1.0F - attenuation }); // white
    }

	Vec3f texcoord = Vec3f({ 0.0, 0.0, 0.0 }); // indicates texture mapping
    Vec3f normal = Vec3f({ 0.0, 0.0, -1.0 }); // indicates face direction
    Vec3f color = Vec3f({ 1.0, 1.0, 1.0 }); // indicates vertex number
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

static Geo_Vertex getMidpoint(std::initializer_list<Geo_Vertex> points){
    Geo_Vertex vertex;

    for(auto p = points.begin(); p != points.end(); p++){
        vertex.position = vertex.position + (*p).position;
        vertex.normal = /* vertex.normal + */ (*p).normal;
        vertex.texcoord = /* vertex.texcoord + */ (*p).texcoord;
        vertex.position = /* vertex.color + */ (*p).color;
    }

    // return Geo_Vertex(vertex.position * (1.0 / points.size()), vertex.texcoord * (1.0 / points.size()), vertex.normal, vertex.texcoord);
    // return Geo_Vertex(vertex.position * (1.0 / points.size()), { 0.5F, 0.5F, 0.5F }, vertex.normal, vertex.texcoord);
    return Geo_Vertex(vertex.position * (1.0 / (float)points.size()));
}

#define GEO_VERTEX_H
#endif
