#define INCLUDE_BLOCK

#include "Common.hlsl"

#define CURSOR_SIZE 0.05
#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 50 // max fractal iteratons
#define C float2(cursorPos.x, cursorPos.y) // c value for julia set

// Values

float4 packedBuffer[4];

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

struct PS_INPUT { float4 pos : SV_POSITION; };

// Functions

/* float3 cursorSet(float2 pos, float2 coord){
	float red = (distance(pos, coord) < 0.01) ? 1.0 : 0.0; // accute center
	float green = pow(1.0 - abs(pos.x - coord.x), 5) - distance(pos, coord); // x gradient
	float blue = pow(1.0 - abs(pos.y - coord.y), 5) - distance(pos, coord); // y gradient
	return float3(red, green, blue);
} */

float4 cursorDot(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) < radius) return color;
    else return float4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

float4 cursorHalo(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return float4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

float4 cursorCross(float2 pos, float2 coord, float radius, float4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1)
        || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return float4(color.r, color.g, color.b, color.a * 0.1); // nearly transparent
}

// Julia set
float3 juliaSet(float2 coord){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = (coord.x * coord.x) - (coord.y * coord.y);
		double y = 2.0 * coord.x * coord.y;
		coord = float2(x, y) + C;
		i++;
	}

	if (i < FRACTAL_ITER) return float3(1.0f / i, tan(i), 0.05f * i); // custom colors outside set
	return float3(0, 0, 0); // black color within set
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

    /* if(mode < 0) return float4(juliaSet((coords - cursor) * abs(mode) * FRACTAL_SIZE), 1.0f);
    else*/ if(mode == 1) return cursorHalo(cursor, coords, CURSOR_SIZE, float4(1.0, 1.0, 1.0, 0.75));
    else if(mode == 2) return cursorCross(cursor, coords, CURSOR_SIZE, float4(1.0, 1.0, 1.0, 0.75));
    else return cursorDot(cursor, coords, CURSOR_SIZE, float4(1.0, 1.0, 1.0, 0.75));
    // else return float4(cursorSet(cursor, coords), 1.0f);
}