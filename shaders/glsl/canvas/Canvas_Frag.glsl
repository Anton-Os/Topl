#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Pixel.glsl"

#define CURSOR_SIZE 0.05

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	ivec2 screenRes; // resolution
	vec2 cursorPos;
    // double drawSize;
	vec2 tracerSteps[TRACER_STEPS];
	vec2 tracerPaths[TRACER_PATHS];
};

layout(location = 0) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

#include "canvas/Canvas.glsl"

// Main

void main() {
    if(mode < 0) color_final = texture(baseTex, vec2(texcoord.x, texcoord.y)); // base texture
    // else color_final = vec4(0.0, 0.0, 0.0, 0.0);

    vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
    vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates
    float size = CURSOR_SIZE * (floor(abs(mode) / 100.0) + 1);

    /* vec4 color_draw;
    if(mode >= 0) color_draw = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // draw
    else color_draw = vec4(0.0, 0.0, 0.0, 0.0); // erase */

    int intersections = 0;

    for(uint t = 0; tracerSteps[t].x != 0.0 && tracerSteps[t].y != 0.0 && t < TRACER_STEPS; t++){
        vec2 step1 = ((tracerSteps[t]) + 1.0) - coords;
        vec2 step2 = ((tracerSteps[t + 1]) + 1.0) - coords;

        float lineDist = getLineDistance(coords, step1, step2);
        vec3 distances = getCoordDistances(coords, step1, step2);

        if(abs(mode) % 10 == 0 && distance(coords, step1) < size) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 1 && intersectLines(lineDist, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 2 && intersectSegments(lineDist, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 3 && intersectRails(lineDist, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 4 && intersectCmp(lineDist, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 5 && intersectTrig(lineDist, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 6 && intersectSwirline(lineDist, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 7 && intersectBlobs(lineDist, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 8 && intersectPow(lineDist, coords, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
        if(abs(mode) % 10 == 9 && intersectStreaks(lineDist, coords, size, distances[0], distances[1], distances[2])) intersections++; // color_final = color_draw;
    }

    if(intersections > 0 && mode >= 0) color_final = modalTex(intersections, texcoord);
    else if(intersections > 0 && mode < 0) color_final = vec4(0.0, 0.0, 0.0, 0.0);

    if(abs(mode) >= 10){
        vec4 color_cursor = vec4(0.0, 0.0, 0.0, 0.0);
        if(uint(floor(abs(mode) / 10.0)) % 10 == 1) color_cursor = cursorDot(cursor, coords, size, vec4(1.0, 1.0, 1.0, 0.75));
        else if(uint(floor(abs(mode) / 10.0)) % 10 == 2) color_cursor = cursorHalo(cursor, coords, size, vec4(1.0, 1.0, 1.0, 0.75));
        else color_cursor = cursorCross(cursor, coords, size, vec4(1.0, 1.0, 1.0, 0.75));

        if(color_cursor.a != 0.0) color_final = color_cursor;
    }
}
