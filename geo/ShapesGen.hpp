#ifndef SHAPESGEN_H

#include "Geometry.hpp"
#include "Geo_Component.hpp"

struct ShapesGen {
    ShapesGen(){}

    // Modify shape through vertex transform callbacks
    static void modShapeFunc(Geo_Component* targetGeo, vertexFuncCallback1 callback, AXIS_Target axis){ // Transofrms a single vertex attribute
        targetGeo->modShapeFunc(callback, axis);
    }
    static void modShapeFunc(Geo_Component* targetGeo, vertexFuncCallback1 xCallback, vertexFuncCallback1 yCallback){ // Transofrms x and y attributes
        targetGeo->modShapeFunc(xCallback, AXIS_X);
        targetGeo->modShapeFunc(yCallback, AXIS_Y);
    }
    static void modShapeFunc(Geo_Component* targetGeo, vertexFuncCallback1 xCallback, vertexFuncCallback1 yCallback, vertexFuncCallback1 zCallback){ // Transofrms x, y, and z attributes
        targetGeo->modShapeFunc(xCallback, AXIS_X);
        targetGeo->modShapeFunc(yCallback, AXIS_Y);
        targetGeo->modShapeFunc(zCallback, AXIS_Z);
    }
    /* static Geo_RenderObj genCompoundShape_add(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_diff(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_intersect(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_exclude(const Geo_Component* firstGeo, const Geo_Component* secondGeo); */
};

#define SHAPESGEN_H
#endif