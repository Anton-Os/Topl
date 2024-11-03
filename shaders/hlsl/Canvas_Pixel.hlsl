#define INCLUDE_TEXTURES
#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

#define CURSOR_SIZE 0.05
#define C float2(cursorPos.x, cursorPos.y) // c value for julia set

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
	float4 steps[4];
	float4 paths[4];
}

static const float2 tracerSteps[8] = (float2[8])steps;
static const float2 tracerPaths[8] = (float2[8])paths;

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 cursorDot(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) < radius) return color;
    else return float4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

float4 cursorHalo(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return float4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

float4 cursorCross(float2 pos, float2 coord, float radius, float4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1) || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return float4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

    // if(mode < 0) return float4(juliaSet((coords - cursor) * abs(mode) * FRACTAL_SIZE), 1.0f);
    if(mode == 1) return cursorHalo(cursor, coords, CURSOR_SIZE, float4(1.0, 1.0, 1.0, 0.75));
    else if(mode == 2) return cursorCross(cursor, coords, CURSOR_SIZE, float4(1.0, 1.0, 1.0, 0.75));
    else return cursorDot(cursor, coords, CURSOR_SIZE, float4(1.0, 1.0, 1.0, 0.75));
    // else return float4(cursorSet(cursor, coords), 1.0f);
}