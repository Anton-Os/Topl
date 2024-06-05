#ifndef GEO_PARAMETRIC_H

#include <initializer_list>

#include "Geo_Mesh.hpp"


typedef Vec3f (*vertex_genCallback)(unsigned short, Vec3f* prevVertex);
typedef unsigned (*index_genCallback)(unsigned short, unsigned* prevIndex);


class Geo_Parametric : public Geo_Mesh {
public:
    Geo_Parametric(unsigned short vertexCount) : Geo_Mesh(vertexCount){}
    Geo_Parametric(unsigned short vertexCount, vertex_genCallback vertexProc) : Geo_Mesh(vertexCount){}
    Geo_Parametric(unsigned short vertexCount, unsigned indexCount) : Geo_Mesh(vertexCount, indexCount){}
    Geo_Parametric(unsigned short vertexCount, vertex_genCallback vertexProc, unsigned indexCount, index_genCallback indexProc) : Geo_Mesh(vertexCount, indexCount){}

protected:
    vertex_genCallback vertexCallback;
    index_genCallback indexCallback;

    void genVertices() override;
	void genIndices() override;
};

struct Geo_ParametricSet : public Geo_Parametric {
    Geo_ParametricSet(std::initializer_list<Vec3f> pointsSet); // Basic Constructor
    Geo_ParametricSet(std::initializer_list<Vec3f> pointsSet, std::initializer_list<Vec3f> pathsSet); // Paths Constructor
    Geo_ParametricSet(std::initializer_list<Vec3f> pointsSet, float scale, unsigned short iterations); // Insertion Constructor
};


#define GEO_PARAMETRIC_H
#endif