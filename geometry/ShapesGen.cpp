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
float contortTform(float input, double rest){ return input; }
float distortTForm(float input, double radii){ return input; }

/* void Geo_ConstructShape::genPos(Vec3f* data){
    return;
}

void Geo_ConstructShape::genNormals(Vec3f* data){
    // Implement Body
    return;
}

void Geo_ConstructShape::genTexCoords(Vec2f* data) {
    // Implement Body
    return;
}

void Geo_ConstructShape::genIndices(unsigned* data){
    // Implement Body
    return;
} */

// Complex Shape Types

void Geo_DuplexShape::genPos(Vec3f* data){
    // Implement Body
    return;
}

void Geo_DuplexShape::genNormals(Vec3f* data){
    // Implement Body
    return;
}

void Geo_DuplexShape::genTexCoords(Vec2f* data) {
    // Implement Body
    return;
}

void Geo_DuplexShape::genIndices(unsigned* data){
    // Implement Body
    return;
}

void Geo_TessShape::genPos(Vec3f* data){
    // Implement Body
    return;
}

void Geo_TessShape::genNormals(Vec3f* data){
    // Implement Body
    return;
}

void Geo_TessShape::genTexCoords(Vec2f* data) {
    // Implement Body
    return;
}

void Geo_TessShape::genIndices(unsigned* data){
    // Implement Body
    return;
}