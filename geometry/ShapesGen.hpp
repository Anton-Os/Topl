#ifndef SHAPESGEN_H

#include "Geometry.hpp"

// Shape Modification Operations

/* class Geo_ConstructShape : public Geo_RenderObj{
public:
    Geo_ConstructShape(renderObj_cptr_t obj1, renderObj_cptr_t obj2) : refObj1(obj1), refObj2(obj2)
    : Geo_RenderObj (
        obj1->getVertexCount() + obj2->getVertexCount(),
        obj1->getIndexCount() + obj2->getIndexCount()){

        fillRenderObj();
    }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
    void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;

    renderObj_cptr_t refObj1;
    renderObj_cptr_t refObj2;
}; */

// Complex Shape Types

class Geo_Iterative { // object for recursive render object generation
public:
    Geo_Iterative(renderObj_cptr_t refObj, unsigned short iterations) : _refObj(refObj){
        _iterations = iterations;
    }
    renderObj_cptr_t getRenderObj(){ return _refObj; }
    unsigned short getIterations(){ return _iterations; }
protected:
    renderObj_cptr_t _refObj;
    unsigned short _iterations = 1;
};

class Geo_DuplexShape : protected Geo_Iterative, public Geo_RenderObj { // shape that generates itself recursively at each vertex
public:
    Geo_DuplexShape(renderObj_cptr_t refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_RenderObj(
        pow(refObj->getVertexCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
        init();
    }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
    void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;
};

class Geo_TessShape : protected Geo_Iterative, public Geo_RenderObj { // shape that recursively breaks itself into tesselated parts
public:
    Geo_TessShape(renderObj_cptr_t refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_RenderObj(
        pow(refObj->getVertexCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
        init();
    }
private:
    void genPos(Vec3f* data) override;
    void genNormals(Vec3f* data) override;
    void genTexCoords(Vec2f* data) override;
    void genIndices(unsigned* data) override;
};

#define SHAPESGEN_H
#endif