#ifndef SHAPESGEN_H

#include "Geo_Mesh.hpp"

// Shape Modification Operations

/* class Geo_ConstructShape : public Geo_Mesh{
public:
    Geo_ConstructShape(const Geo_Mesh* obj1, const Geo_Mesh* obj2) : refObj1(obj1), refObj2(obj2)
    : Geo_Mesh (
        obj1->getVertexCount() + obj2->getVertexCount(),
        obj1->getIndexCount() + obj2->getIndexCount()){
    }
private:
    void genVertices() override;
	void genIndices() override;
}; */

// Complex Shape Types

class Geo_Iterative { // object for recursive render object generation
public:
    Geo_Iterative(const Geo_Mesh* refObj, unsigned short iterations) : _refObj(refObj){
        _iterations = iterations;
    }
    const Geo_Mesh* getMesh(){ return _refObj; }
    unsigned short getIterations(){ return _iterations; }
protected:
    const Geo_Mesh* _refObj;
    unsigned short _iterations = 1;
};

class Geo_DuplexShape : protected Geo_Iterative, public Geo_Mesh { // shape that generates itself recursively at each vertex
public:
    Geo_DuplexShape(const Geo_Mesh* refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_Mesh(
        pow(refObj->getVertexCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
    }
private:
	void genVertices() override;
	void genIndices() override;
};

class Geo_TessShape : protected Geo_Iterative, public Geo_Mesh { // shape that recursively breaks itself into tesselated parts
public:
    Geo_TessShape(const Geo_Mesh* refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_Mesh(
        pow(refObj->getVertexCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
    }
private:
	void genVertices() override;
	void genIndices() override;
};

#define SHAPESGEN_H
#endif