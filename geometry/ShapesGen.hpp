#ifndef SHAPESGEN_H

#include "Geometry.hpp"

// Shape Modification Operations

/* class Geo_ConstructShape : public Geo_Renderable{
public:
    Geo_ConstructShape(const Geo_Renderable* obj1, const Geo_Renderable* obj2) : refObj1(obj1), refObj2(obj2)
    : Geo_Renderable (
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
    Geo_Iterative(const Geo_Renderable* refObj, unsigned short iterations) : _refObj(refObj){
        _iterations = iterations;
    }
    const Geo_Renderable* getRenderable(){ return _refObj; }
    unsigned short getIterations(){ return _iterations; }
protected:
    const Geo_Renderable* _refObj;
    unsigned short _iterations = 1;
};

class Geo_DuplexShape : protected Geo_Iterative, public Geo_Renderable { // shape that generates itself recursively at each vertex
public:
    Geo_DuplexShape(const Geo_Renderable* refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_Renderable(
        pow(refObj->getVertexCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
    }
private:
	void genVertices() override;
	void genIndices() override;
};

class Geo_TessShape : protected Geo_Iterative, public Geo_Renderable { // shape that recursively breaks itself into tesselated parts
public:
    Geo_TessShape(const Geo_Renderable* refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_Renderable(
        pow(refObj->getVertexCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
    }
private:
	void genVertices() override;
	void genIndices() override;
};

#define SHAPESGEN_H
#endif