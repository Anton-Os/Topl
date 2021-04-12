#include "Geometry.hpp"
#include "Geo_Component.hpp"

enum AXIS_Target{
    AXIS_X,
    AXIS_Y,
    AXIS_Z
}

typedef float (*vertexFuncCallback)(float); // Accepts a vertex attribute and creates a new one

struct ShapesGen {
    ShapesGen(){}

    static Geo_RenderObj(const Geo_Component* targetGeo, vertexFuncCallback callback, AXIS_Target axis); // Transofrms a single vertex attribute
    static Geo_RenderObj(const Geo_Component* targetGeo, vertexFuncCallback xCallback, vertexFuncCallback yCallback); // Transofrms x and y attributes
    static Geo_RenderObj(const Geo_Component* targetGeo, vertexFuncCallback xCallback, vertexFuncCallback yCallback, vertexFuncCallback zCallback); // Transofrms x, y, and z attributes

    static Geo_RenderObj genCompoundShape_add(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_diff(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_intersect(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
    static Geo_RenderObj genCompoundShape_exclude(const Geo_Component* firstGeo, const Geo_Component* secondGeo);
};

#define SHAPESGEN_H
#endif