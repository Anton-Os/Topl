#ifndef SHAPESGEN_H

#include "Geometry.hpp"

struct ShapesGen {
    ShapesGen(){}

    // Modify shape through vertex transform callbacks
    static void modify(Geo_RenderObj* renderObj, double mod, vTransformCallback callback, AXIS_Target axis);
    static void modify(Geo_RenderObj* renderObj, double mod, vTransformCallback xCallback, vTransformCallback yCallback);
    static void modify(Geo_RenderObj* renderObj, double mod, vTransformCallback xCallback, vTransformCallback yCallback, vTransformCallback zCallback);
    // Object Creation Utilities
    static void genColliders(Phys_Colliders* colliders, const std::vector<Geo_Face> *const faces); // generates a collision object based on faces provided
    // Tiling Functions
    static void tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Square tiling
    static void tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Triangular tiling
    static void tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Hexagonal tiling
    /* static Geo_RenderObj genCompoundShape_add(const Geo_RenderObj* firstGeo, const Geo_RenderObj* secondGeo);
    static Geo_RenderObj genCompoundShape_diff(const Geo_RenderObj* firstGeo, const Geo_RenderObj* secondGeo);
    static Geo_RenderObj genCompoundShape_intersect(const Geo_RenderObj* firstGeo, const Geo_RenderObj* secondGeo);
    static Geo_RenderObj genCompoundShape_exclude(const Geo_RenderObj* firstGeo, const Geo_RenderObj* secondGeo); */
};

#define SHAPESGEN_H
#endif