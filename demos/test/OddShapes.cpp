// #include "Topl_Image.hpp"

#include "primitives/Geo_Sphere.hpp"
#include "ShapesGen.hpp"

#define ITERATIONS 6

void main(){
    NGon3D ngon = { 0.1, 6, 6 };
    Geo_SphereUV sphere = Geo_SphereUV(ngon);

    Geo_DuplexShape = Geo_DuplexShape((Geo_RenderObj*)&sphere, ITERATIONS);
    Geo_TessShape tessShape = Geo_TessShape((Geo_RenderObj*)&sphere, ITERATIONS);

    return;
}