#include "primitives/Geo_Sphere.hpp"
#include "ShapesGen.hpp"

#define ITERATIONS 6

int main(){
    Shape3D ngon = { 0.1, 6, 6 };
    Geo_Sphere sphere = Geo_Sphere(ngon);

    Geo_DuplexShape duplexShape = Geo_DuplexShape((Geo_RenderObj*)&sphere, ITERATIONS);
    Geo_TessShape tessShape = Geo_TessShape((Geo_RenderObj*)&sphere, ITERATIONS);

    return 0;
}