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

// Distance Functions

float getLineDistance(vec2 coord, vec2 p1, vec2 p2){
	return abs(((p2.y - p1.y) * coord.x) - ((p2.x - p1.x) * coord.y) + (p2.x * p1.y) - (p2.y * p1.x)) / sqrt(pow(p2.y - p1.y, 2.0) + pow(p2.x - p1.x, 2.0));
}

vec3 getCoordDistances(vec2 coord, vec2 p1, vec2 p2){
	return vec3(
		sqrt(pow(p2.x - p1.x, 2.0) + pow(p2.y - p1.y, 2.0)), // distance between points 1 and 2
		sqrt(pow(coord.x - p1.x, 2.0) + pow(coord.y - p1.y, 2.0)), // distance between coordinate and point 1 
		sqrt(pow(coord.x - p2.x, 2.0) + pow(coord.y - p2.y, 2.0)) // distance between coordinate and point 2
	);
}

// Cursor Functions

vec4 cursorDot(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) < radius) return color;
    else return vec4(color.r, color.g, color.b, 0.0);
}

vec4 cursorHalo(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return vec4(color.r, color.g, color.b, 0.0);
}

vec4 cursorCross(vec2 pos, vec2 coord, float radius, vec4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1) || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return vec4(color.r, color.g, color.b, 0.0);
}

// Draw Functions

bool intersectLines(float lineDist, float size, float endpointDist, float dist1, float dist2){
    return lineDist < size && dist1 < endpointDist && dist2 < endpointDist;
}

bool intersectSegments(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return (abs(dist1 - dist2) * 10) - floor(abs(dist1 - dist2) * 10) < 0.5;
    else return false;
}

bool intersectRails(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return cos(lineDist * 100) < 0;
    else return false;
}

bool intersectCmp(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return dist1 / lineDist > dist2 / dist1;
    else return false;
}

bool intersectTrig(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return sin((dist1 + dist2) * 100) > tan((dist1 + dist2) * 100);
    else return false;
}

bool intersectSwirline(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return tan(dist1 / dist2) < 0.0;
    else return false;
}

bool intersectBlobs(float lineDist, float endpointDist, float dist1, float dist2){
    return lineDist + sin(dist1) * cos(dist2) < tan(endpointDist / (dist1 + dist2)) && dist1 < endpointDist && dist2 < endpointDist;
}

bool intersectStreaks(float lineDist, vec2 coord, float size, float endpointDist, float dist1, float dist2){
    return lineDist * distance(cursorPos, coord) < size && dist1 < endpointDist && dist2 < endpointDist;
}

bool intersectPow(float lineDist, vec2 coord, float size, float endpointDist, float dist1, float dist2){
    return pow(lineDist, coord.x) * dist1 > pow(lineDist, coord.y) * dist2 && dist1 < endpointDist && dist2 < endpointDist;
}

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
