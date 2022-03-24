#ifndef SHAPESGEN_H

#include "Geometry.hpp"

// Shape Modification Operations

static void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis);
static void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback);
static void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback);

float shiftTForm(float input, double amount); // shifts point by amount
float stretchTForm(float input, double factor); // stretches point by factor
float contortTform(float input, double rest); // equalizes values to rest value
float distortTForm(float input, double radii); // distorts value based on concentric radii

// Complex Shape Types

class Geo_Iterative { // object for recursive render object generation
public:
    Geo_Iterative(const Geo_RenderObj* refObj, unsigned short iterations){
        _refObj = refObj;
        _iterations = iterations;
    }
    const Geo_RenderObj* getRenderObj(){ return _refObj; }
    unsigned short getIterations(){ return _iterations; }
protected:
    const Geo_RenderObj* _refObj = nullptr;
    unsigned short _iterations = 1;
};

class Geo_DuplexShape : protected Geo_Iterative, public Geo_RenderObj { // shape that generates itself recursively at each vertex
public:
    Geo_DuplexShape(const Geo_RenderObj* refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_RenderObj
    (pow(refObj->getVerticesCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
        fillRenderObj();
    }
private:
    void genPos(Eigen::Vector3f* data) override;
    void genNormals(Eigen::Vector3f* data) override;
    void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
};

class Geo_TessShape : protected Geo_Iterative, public Geo_RenderObj { // shape that recursively breaks itself into tesselated parts
public:
    Geo_TessShape(const Geo_RenderObj* refObj, unsigned short iterations)
    : Geo_Iterative(refObj, iterations),
    Geo_RenderObj
    (pow(refObj->getVerticesCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
        fillRenderObj();
    }
private:
    void genPos(Eigen::Vector3f* data) override;
    void genNormals(Eigen::Vector3f* data) override;
    void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
};

#define SHAPESGEN_H
#endif