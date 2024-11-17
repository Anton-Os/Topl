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

// Main

float4 main(PS_INPUT input) : SV_TARGET{
    float4 color_out = float4(0.0, 0.0, 0.0, 0.0);
    if(mode >= 0) color_out = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // show texture if mode is positive

	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates
    float size = CURSOR_SIZE * (floor(abs(mode) / 100.0) + 1);

    if(mode % 10 != 0){
        // TODO: Perform drawing and path operations
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