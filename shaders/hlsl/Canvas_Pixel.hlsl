#define INCLUDE_TEXTURES
#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

#define CURSOR_SIZE 0.05
#define TRACER_STEPS 8
#define TRACER_PATHS 8

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
	float4 steps[TRACER_STEPS / 2];
	float4 paths[TRACER_PATHS / 2];
}

static const float2 tracerSteps[TRACER_STEPS] = (float2[TRACER_STEPS])steps;
static const float2 tracerPaths[TRACER_PATHS] = (float2[TRACER_PATHS])paths;

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Cursor Functions

float4 cursorDot(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) < radius) return color;
    else return float4(color.r, color.g, color.b, 0.0);
}

float4 cursorHalo(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return float4(color.r, color.g, color.b, 0.0);
}

float4 cursorCross(float2 pos, float2 coord, float radius, float4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1) || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return float4(color.r, color.g, color.b, 0.0);
}

// Draw Functions

uint intersectLines(float2 pos, float2 coord, float dist){
    uint i = 0;

    for(uint t = 0; tracerPaths[t].x != 0.0 && tracerPaths[t].y != 0.0 && tracerPaths[t + 1].x != 0.0 && tracerPaths[t + 1].y != 0.0 && t < TRACER_PATHS; t++){
        float2 path1 = ((tracerPaths[t] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f - coord;
        float2 path2 = ((tracerPaths[t + 1] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f - coord;

        float lineDist = getLineDistance(coord, path1, path2);
        float3 distances = getCoordDistances(coord, path1, path2);
        if(lineDist < dist && distances[1] < distances[0] && distances[2] < distances[0]) i++;
    }

    return i;
}

uint intersectCurves(float2 pos, float2 coord, float dist){
    uint i = 0;

    for(uint t = 0; tracerPaths[t].x != 0.0 && tracerPaths[t].y != 0.0 && tracerPaths[t + 1].x != 0.0 && tracerPaths[t + 1].y != 0.0 && t < TRACER_PATHS; t++){
        float2 path1 = ((tracerPaths[t] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f - coord;
        float2 path2 = ((tracerPaths[t + 1] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f - coord;

        float lineDist = getLineDistance(coord, path1, path2);
        float3 distances = getCoordDistances(coord, path1, path2);
        if(lineDist + sin(distances[1]) + sin(distances[2]) < dist && distances[1] < distances[0] && distances[2] < distances[0]) i++;
    }

    return i;
}

uint intersectZigZags(float2 pos, float2 coord, float dist){
    uint i = 0;

    for(uint t = 0; tracerPaths[t].x != 0.0 && tracerPaths[t].y != 0.0 && tracerPaths[t + 1].x != 0.0 && tracerPaths[t + 1].y != 0.0 && t < TRACER_PATHS; t++){
        float2 path1 = ((tracerPaths[t] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f - coord;
        float2 path2 = ((tracerPaths[t + 1] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f - coord;

        float lineDist = getLineDistance(coord, path1, path2);
        float3 distances = getCoordDistances(coord, path1, path2);
        if(lineDist * distance(pos, coord) < dist && distances[1] < distances[0] && distances[2] < distances[0]) i++;
    }

    return i;
}

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates
    float size = CURSOR_SIZE * (floor(abs(mode) / 100.0) + 1);

    // Drawing

    float4 color_out = float4(0.0, 0.0, 0.0, 0.0); // blank canvas
    if(mode < 0) color_out = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // full canvas

    uint intersections = 0;
    if(abs(mode) % 10 == 1) intersections = intersectLines(cursor, coords, size);
    else if(abs(mode) % 10 == 2) intersections = intersectCurves(cursor, coords, size);
    else if(abs(mode) % 10 == 3) intersections = intersectZigZags(cursor, coords, size);

    if(intersections > 0 && mode >= 0) color_out = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // draw on canvas
    else if(intersections > 0 && mode < 0) color_out = float4(0.0, 0.0, 0.0, 0.0); // erase canvas

    // Cursor

    if(abs(mode) >= 10){
        float4 color_cursor = float4(0.0, 0.0, 0.0, 0.0);
        if(floor(abs(mode) / 10.0) % 10 == 1) color_cursor = cursorDot(cursor, coords, size, float4(1.0, 1.0, 1.0, 0.75));
        else if(floor(abs(mode) / 10.0) % 10 == 2) color_cursor = cursorCross(cursor, coords, size, float4(1.0, 1.0, 1.0, 0.75));
        else color_cursor = cursorHalo(cursor, coords, size, float4(1.0, 1.0, 1.0, 0.75));

        if(color_cursor.a != 0.0) color_out = color_cursor;
    }

    return color_out;
}