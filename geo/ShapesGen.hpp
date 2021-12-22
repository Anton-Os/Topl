#ifndef SHAPESGEN_H

#include "Geometry.hpp"

struct ShapesGen {
    ShapesGen(){}

    // Modify shape through vertex transform callbacks
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis);
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback);
    static void modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback);
    // Render Object Generation
    static void genTessShape(Geo_RenderObj* newObj, const Geo_RenderObj* refObj, unsigned iter); // tesselates objects by iterating through triangle midpoints
    static void genDuplexShape(Geo_RenderObj* newObj, const Geo_RenderObj* refObj, Geo_RenderObj* duplexObj); // inserts duplex object at every vertex of ref object
    // Tiling Functions
    /* static void tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Square tiling
    static void tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Triangular tiling
    static void tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Hexagonal tiling */
};

#define SHAPESGEN_H
#endif