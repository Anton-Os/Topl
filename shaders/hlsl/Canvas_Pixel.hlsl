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

float4 drawLines(float2 pos, float2 coord, float dist, float4 color){;
    uint t = 0;
    float4 color_draw = float4(color.r, color.g, color.b, 0.0);

    while(tracerPaths[t].x != 0.0 && tracerPaths[t].y != 0.0 && t < TRACER_PATHS){
        float2 path1 = ((tracerPaths[t] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f;
        float2 path2 = ((tracerPaths[t + 1] * float2(1.0f, -1.0f)) * 0.5f) + 1.0f;
        
        float x1 = path1.x - coord.x; float y1 = path1.y - coord.y;
        float x2 = path2.x - coord.x; float y2 = path2.y - coord.y;

        float lineDist = abs(((y2 - y1) * coord.x) - ((x2 - x1) * coord.y) + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1, 2.0) + pow(x2 - x1, 2.0));
        float3 pointDists = float3(distance(float2(x1, y1), float2(x2, y2)), distance(coord, float2(x1, y1)), distance(coord, float2(x2, y2)));
        if(lineDist < dist && pointDists[1] < pointDists[0] && pointDists[2] < pointDists[0]) color_draw = color;

        t++;
    }

    return color_draw;
}

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
    float4 color_out = float4(0.0, 0.0, 0.0, 0.0);
    if(mode >= 0) color_out = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // show texture if mode is positive

	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates
    float size = CURSOR_SIZE * (floor(abs(mode) / 100.0) + 1);

    if(abs(mode) % 10 != 0){
        float4 color_draw = float4(0.0, 0.0, 0.0, 0.0);
        if(abs(mode) % 10 == 1) color_draw = drawLines(cursor, coords, size, float4(input.texcoord, 0.5));
        
        if(color_draw.a != 0.0) color_out = color_draw;
    }

    if(abs(mode) >= 10){ // adding cursor color
        float4 color_cursor = float4(0.0, 0.0, 0.0, 0.0);
        if(floor(abs(mode) / 10.0) % 10 == 1) color_cursor = cursorDot(cursor, coords, size, float4(1.0, 1.0, 1.0, 0.75));
        else if(floor(abs(mode) / 10.0) % 10 == 2) color_cursor = cursorCross(cursor, coords, size, float4(1.0, 1.0, 1.0, 0.75));
        else color_cursor = cursorHalo(cursor, coords, size, float4(1.0, 1.0, 1.0, 0.75));

        if(color_cursor.a != 0.0) color_out = color_cursor;
    }

    return color_out;
}