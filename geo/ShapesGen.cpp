#include "ShapesGen.hpp"

// Shape Modification Operations

void ShapesGen::modify(Geo_RenderObj* renderObj, double mod, vTransformCallback callback, AXIS_Target axis){
    renderObj->modify(callback, mod, axis);
}

void ShapesGen::modify(Geo_RenderObj* renderObj, double mod, vTransformCallback xCallback, vTransformCallback yCallback){ // Transofrms x and y attributes
    renderObj->modify(xCallback, mod, AXIS_X);
    renderObj->modify(yCallback, mod, AXIS_Y);
}

void ShapesGen::modify(Geo_RenderObj* renderObj, double mod, vTransformCallback xCallback, vTransformCallback yCallback, vTransformCallback zCallback){ // Transofrms x, y, and z attributes
    renderObj->modify(xCallback, mod, AXIS_X);
    renderObj->modify(yCallback, mod, AXIS_Y);
    renderObj->modify(zCallback, mod, AXIS_Z);
}

// Tiling Operations

void ShapesGen::tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations){
    return; // implement here
}

void ShapesGen::tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations){
    return; // implement here
}

void ShapesGen::tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations){
    return; // implement here
}