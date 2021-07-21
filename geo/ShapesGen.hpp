#ifndef SHAPESGEN_H

#include "Geometry.hpp"
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
    /* static Geo_RenderObj genCompoundShape_add(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_diff(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_intersect(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_exclude(const Geo_Component* firstGeo, const Geo_Component* secondGeo); */
};

#define SHAPESGEN_H
#endif