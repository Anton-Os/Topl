#define INCLUDE_BLOCK

#include "Common.hlsl"

#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 50 // max fractal iteratons
#define C float2(cursorPos.x, cursorPos.y) // c value for julia set

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
}

struct PS_INPUT { float4 pos : SV_POSITION; };

// Functions

// Mandlebrot Set
float3 mandlebrotSet(float2 coord){
	uint i = 0; // iteration count
	double x = 0; double y = 0;

	while(x * x + y * y <= FRACTAL_SIZE && i < FRACTAL_ITER){
		double temp = (x * x) - (y * y) + coord.x;
		y = (2 * x * y) + coord.y;
		x = temp;
		i++;
	}

	if(i < FRACTAL_ITER) return float3(0.05f * i, sin(i), 1.0f / i); // custom colors outside set
	else return float3(0.0f, 0.0f, 0.0f); // black color within set
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

    if(mode >= 10 && mode < 20) return float4(juliaSet((coords - cursor) * FRACTAL_SIZE), 1.0f);
    else return  float4(mandlebrotSet((coords - cursor) * FRACTAL_SIZE), 1.0f);
    // else return float4(cursorSet(cursor, coords), 1.0f);
}