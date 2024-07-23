#define INCLUDE_BLOCK

#include "Common.hlsl"

#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 100 // max fractal iteratons
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

float3 cursorSet(float2 pos, float2 coord){
	float red = (distance(pos, coord) < 0.01) ? 1.0 : 0.0; // accute center
	float green = pow(1.0 - abs(pos.x - coord.x), 5) - distance(pos, coord); // x gradient
	float blue = pow(1.0 - abs(pos.y - coord.y), 5) - distance(pos, coord); // y gradient
	return float3(red, green, blue);
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

float3 bubbleSet(float2 coord1, float2 coord2){
    uint i = 0; // iteration count

    float3 target = float3(0.5, 0.5, 0.5);
    while(((1.0 / coord1.x) * (1.0 / coord1.y) > pow(coord2.x * coord2.y, 1.0) || target.b == 0.5) && i < 100){
        coord1.x += pow(coord2.y, i);
        coord1.y += pow(coord2.x, i);
        target = float3(1.0 / coord1.x, 1.0 / coord1.y, pow(coord1.x * coord1.y, coord2.x * coord2.y));
        i++;
    }

    return target;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

	if (mode == 1) return float4(cursorSet(cursor, coords), 1.0f); // cursor track mode
	else if(mode == 2) return float4(bubbleSet(tracerSteps[0], coords), 1.0f);
	else return float4(juliaSet((coords - cursor) * FRACTAL_SIZE), 1.0f); // fractal mode
}