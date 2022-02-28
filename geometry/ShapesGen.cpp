#include "ShapesGen.hpp"

// Shape Modification Operations

void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback callback, AXIS_Target axis){
    renderObj->modify(callback, mod, axis);
}

void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback){ // Transofrms x and y attributes
    renderObj->modify(xCallback, mod, AXIS_X);
    renderObj->modify(yCallback, mod, AXIS_Y);
}

void modifyShape(Geo_RenderObj* renderObj, double mod, vTformCallback xCallback, vTformCallback yCallback, vTformCallback zCallback){ // Transofrms x, y, and z attributes
    renderObj->modify(xCallback, mod, AXIS_X);
    renderObj->modify(yCallback, mod, AXIS_Y);
    renderObj->modify(zCallback, mod, AXIS_Z);
}

float shiftTForm(float input, double amount){ return input + amount; }
float stretchTForm(float input, double factor){ return input * factor; }
float spinTForm(float input, double angle){ return input; } // implement
float waveTform(float input, double rest){ return input; } // implement
float starTForm(float input, double circles){ return input; } // implement

// Complex Shape Types

void Geo_DuplexShape::genPos(Eigen::Vector3f* data){
    // Implement Body
    return;
}

void Geo_DuplexShape::genNormals(Eigen::Vector3f* data){
    // Implement Body
    return;
}

void Geo_DuplexShape::genTexCoords(Eigen::Vector2f* data) {
    // Implement Body
    return;
}

void Geo_DuplexShape::genIndices(unsigned* data){
    // Implement Body
    return;
}


void Geo_TessShape::genPos(Eigen::Vector3f* data){
    // Implement Body
    return;
}

void Geo_TessShape::genNormals(Eigen::Vector3f* data){
    // Implement Body
    return;
}

void Geo_TessShape::genTexCoords(Eigen::Vector2f* data) {
    // Implement Body
    return;
}

void Geo_TessShape::genIndices(unsigned* data){
    // Implement Body
    return;
}