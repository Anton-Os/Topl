#ifndef SHAPESGEN_H

#include "Geometry.hpp"

// Shape Modification Operations

static void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis);
static void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback);
static void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback);

float shiftTForm(float input, double amount); // shifts point by amount
float stretchTForm(float input, double factor); // stretches point by factor
float spinTForm(float input, double angle); // rotates point by anchor
float waveTform(float input, double rest); // makes point equalize to rest value
float starTForm(float input, double circles); // distorts point based on concentric circles

// Complex Shape Types

class Geo_Iterative { // helper object for recursive render object generation
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


// Tiling Functions
/* static void tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Square tiling
static void tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Triangular tiling
static void tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Hexagonal tiling */

#define SHAPESGEN_H
#endif