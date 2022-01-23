#ifndef SHAPESGEN_H

#include "Geometry.hpp"

struct ShapesGen {
    class Geo_DuplexShape : public Geo_RenderObj { // shape that generates itself recursively
    public:
        Geo_DuplexShape(const Geo_RenderObj* refObj, unsigned short iterations)
        : Geo_RenderObj
        (pow(refObj->getVerticesCount(), iterations),
        pow(refObj->getIndexCount(), iterations)){
            _refObj = refObj;
            _iterations = iterations;
            fillRenderObj();
        }

        const Geo_RenderObj* getRenderObj(){ return _refObj; }
        unsigned short getIterations(){ return _iterations; }
    private:
        void genVertices(Eigen::Vector3f* data) override;
        void genNormals(Eigen::Vector3f* data) override;
        void genTexCoords(Eigen::Vector2f* data) override;
        void genIndices(unsigned* data) override;

        const Geo_RenderObj* _refObj = nullptr;
        unsigned short _iterations = 1;
    };

    // Modify shape through vertex transform callbacks
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis);
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback);
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback);
    // Render Object Generation
    /* static Geo_RenderObj genTessShape(const Geo_RenderObj* refObj, unsigned iter); // tesselates objects by iterating through triangle midpoints
    static Geo_RenderObj genDuplexShape(const Geo_RenderObj* refObj, Geo_RenderObj* duplexObj); // inserts duplex object at every vertex of ref object */
    // Tiling Functions
    /* static void tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Square tiling
    static void tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Triangular tiling
    static void tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Hexagonal tiling */
};

#define SHAPESGEN_H
#endif