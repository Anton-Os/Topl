#include "Maths.h"

int main(void){
    Vec3f vec1 = { 0.0f, 0.0f, 0.0f };
    Vec3f vec2 = vec1 + Vec3f({ 0.0f, 0.5f, 0.0f });
    Vec3f vec3 = vec1 * 5.0;
    Vec2f vec4 = Vec2f({1.0f, -1.0f}) - Vec2f({-1.0f, 1.0f});
    Vec3f vec5 = vec2 * vec2;
    Vec3f vec6 = vec5.normalize();
    float len = vec6.len();
    Vec4i vec7 = Vec4i({ 0xFF, 0xFF, 0xFF, 0xFF});

    Mat4x4 identity = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return 0;
}