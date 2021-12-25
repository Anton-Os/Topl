#include "ShapesGen.hpp"

// Shape Modification Operations

void ShapesGen::modify(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis){
    renderObj->modify(callback, mod, axis);
}

void ShapesGen::modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback){ // Transofrms x and y attributes
    renderObj->modify(xCallback, mod, AXIS_X);
    renderObj->modify(yCallback, mod, AXIS_Y);
}

void ShapesGen::modify(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback){ // Transofrms x, y, and z attributes
    renderObj->modify(xCallback, mod, AXIS_X);
    renderObj->modify(yCallback, mod, AXIS_Y);
    renderObj->modify(zCallback, mod, AXIS_Z);
}

// Render Object Generation

/* Geo_RenderObj ShapesGen::genTessShape(Geo_RenderObj* newObj, const Geo_RenderObj* refObj, unsigned iter){
    // Implement body
}

Geo_RenderObj ShapesGen::genDuplexShape(Geo_RenderObj* newObj, const Geo_RenderObj* refObj, Geo_RenderObj* duplexObj){
    // Implement body
} */

// Tiling Operations

/* void ShapesGen::tileSquare(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations){
    return; // implement here
}

void ShapesGen::tileTrig(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations){
    return; // implement here
}

void ShapesGen::tileHex(std::vector<Eigen::Vector3f>* posData, Geo_Face face, unsigned iterations){
    return; // implement here
} */