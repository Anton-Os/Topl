#define INCLUDE_TEXTURES
#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

#include "Pixel.hlsl"

#define CURSOR_SIZE 0.05

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
    // double drawSize;
	float4 steps[TRACER_STEPS / 2];
	float4 paths[TRACER_PATHS / 2];
}

static const float2 tracerSteps[TRACER_STEPS] = (float2[TRACER_STEPS])steps;
static const float2 tracerPaths[TRACER_PATHS] = (float2[TRACER_PATHS])paths;

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

#include "canvas/Canvas.hlsl"

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f))) + 0.5f;
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates
    float size = CURSOR_SIZE * (floor(abs(mode) / 100.0) + 1);

    // Drawing

    float4 color_out = float4(0.0, 0.0, 0.0, 0.0); // blank canvas
    if(mode < 0) color_out = baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y)); // full canvas

    uint intersections = 0;

    for(uint t = 0; tracerSteps[t].x != 0.0 && tracerSteps[t].y != 0.0 && t < TRACER_STEPS; t++){
        float2 step1 = ((tracerSteps[t]) + 1.0) - coords;
        float2 step2 = ((tracerSteps[t + 1]) + 1.0) - coords;

        float lineDist = getLineDistance(coords, step1, step2);
        float3 distances = getCoordDistances(coords, step1, step2);

        if(abs(mode) % 10 == 0 && distance(coords, step1) < size) intersections++;
        if(abs(mode) % 10 == 1 && intersectLines(lineDist, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 2 && intersectSegments(lineDist, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 3 && intersectRails(lineDist, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 4 && intersectCmp(lineDist, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 5 && intersectTrig(lineDist, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 6 && intersectSwirline(lineDist, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 7 && intersectBlobs(lineDist, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 8 && intersectPow(lineDist, coords, size, distances[0], distances[1], distances[2])) intersections++;
        if(abs(mode) % 10 == 9 && intersectStreaks(lineDist, coords, size, distances[0], distances[1], distances[2])) intersections++;
    
        if(intersections > 0 && mode >= 0) color_out = getStepColor(intersections) * (1.0 - lineDist); // modalTex(intersections, input.texcoord);
        else if(intersections > 0 && mode < 0) color_out = float4(0.0, 0.0, 0.0, 0.0); // erase canvas
    }

    // Cursor

    if(abs(mode) >= 10){
        float4 color_cursor = float4(0.0, 0.0, 0.0, 0.0);
        float2 cursorAdj = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
        if(floor(abs(mode) / 10.0) % 10 == 1) color_cursor = cursorDot(cursorAdj, coords, size, float4(1.0, 1.0, 1.0, 0.75));
        else if(floor(abs(mode) / 10.0) % 10 == 2) color_cursor = cursorCross(cursorAdj, coords, size, float4(1.0, 1.0, 1.0, 0.75));
        else color_cursor = cursorHalo(cursorAdj, coords, size, float4(1.0, 1.0, 1.0, 0.75));

        if(color_cursor.a != 0.0) color_out = color_cursor;
    }

    return color_out;
}