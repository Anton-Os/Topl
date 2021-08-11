#ifndef SHAPESGEN_H

#include "Geo_Component.hpp"

struct ShapesGen {
    ShapesGen(){}

    // Modify shape through vertex transform callbacks
    static void modify(Geo_Component* targetGeo, vTransformCallback callback, AXIS_Target axis){ // Transofrms a single vertex attribute
        targetGeo->modify(callback, axis);
    }
    static void modify(Geo_Component* targetGeo, vTransformCallback xCallback, vTransformCallback yCallback){ // Transofrms x and y attributes
        targetGeo->modify(xCallback, AXIS_X);
        targetGeo->modify(yCallback, AXIS_Y);
    }
    static void modify(Geo_Component* targetGeo, vTransformCallback xCallback, vTransformCallback yCallback, vTransformCallback zCallback){ // Transofrms x, y, and z attributes
        targetGeo->modify(xCallback, AXIS_X);
        targetGeo->modify(yCallback, AXIS_Y);
        targetGeo->modify(zCallback, AXIS_Z);
    }
    // Object Creation Utilities
    static void genColliders(Phys_Colliders* colliders, const std::vector<Geo_Face> *const faces); // generates a collision object based on faces provided
    // Tiling Functions
    static void tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Square tiling
    static void tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Triangular tiling
    static void tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations); // Hexagonal tiling
    /* static Geo_RenderObj genCompoundShape_add(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_diff(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_intersect(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_exclude(const Geo_Component* firstGeo, const Geo_Component* secondGeo); */
};

#define SHAPESGEN_H
#endif