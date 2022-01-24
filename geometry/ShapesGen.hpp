#ifndef SHAPESGEN_H

#include "Geometry.hpp"

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

struct ShapesGen {
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
        void genVertices(Eigen::Vector3f* data) override;
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
        void genVertices(Eigen::Vector3f* data) override;
        void genNormals(Eigen::Vector3f* data) override;
        void genTexCoords(Eigen::Vector2f* data) override;
        void genIndices(unsigned* data) override;
    };

    // Modify shape through vertex transform callbacks
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis);
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback);
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback);
    // Tiling Functions
    /* static void tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Square tiling
    static void tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Triangular tiling
    static void tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Hexagonal tiling */
};

#define SHAPESGEN_H
#endif