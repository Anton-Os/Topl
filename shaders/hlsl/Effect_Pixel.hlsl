#define INCLUDE_BLOCK

#include "Common.hlsl"

#define FRACTAL_SIZE 5.0 // max fractal size
#define FRACTAL_ITER 20 // max fractal iteratons

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
}

struct PS_INPUT { 
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float3 fractalColors(float2 coord, float2 cursor, uint i){
	if(mode % 10 == 0) return float3(1.0f / i, tan(i), 0.05f * i);
	else if(mode % 10 == 1) return float3(getRandColor(i).r, getRandColor(i).g, getRandColor(i).b);
	else if(mode % 10 == 2) return float3(pow(coord.x, i), pow(coord.y, 1.0 / i), pow(coord.x, coord.y));
	else if(mode % 10 == 3) return float3(distance(coord - cursor, float2(0.0, 0.0)), distance(cursor, float2(0.0, 0.0)), distance(coord, cursor));
	else if(mode % 10 == 4) return float3(coord.x * i - floor(coord.x * i), ceil(coord.y * i) - coord.y * i, abs(dot(cursor, coord)) - floor(abs(dot(cursor, coord))));
	else return float3(coord.x, coord.y, 1.0 / i);
	// TODO: Include more color options
}

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

	if(i < FRACTAL_ITER) return fractalColors(float2(x, y), cursorPos, i);
	else return float3(0.0f, 0.0f, 0.0f); // black color within set
}

// Julia set
float3 juliaSet(float2 coord, float2 cursor){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = (coord.x * coord.x) - (coord.y * coord.y);
		double y = 2.0 * coord.x * coord.y;
		coord = float2(x, y) + cursor;
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return float3(0, 0, 0); // black color within set
}

// Trig set
float3 trigSet(float2 coord){
	uint i = 0; // iteration count

	/* while (atan(coord.x) + tan(coord.y) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = coord.x + sin(coord.y / coord.x);
		double y = coord.y + cos(coord.x / coord.y);
		coord = float2(x, y);
		i++;
	} */

	while(abs(sin(coord.x) + cos(coord.y)) * abs(1.0 / (coord.x * coord.y)) < FRACTAL_SIZE && i < FRACTAL_ITER){
		double x = coord.x + sin(coord.y);
		double y = coord.y + cos(coord.x);
		coord = float2(x, y);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursorPos, i);
	return float3(0, 0, 0); // black color within set
}

// Power Set
float3 powerSet(float2 coord, float2 cursor){
	uint i = 0; // iteration count

	// while(pow(pow(abs(coord.x + coord.y), abs(coord.x - coord.y)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	while(pow(pow(abs(coord.x / coord.y), abs(coord.y / coord.x)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	// while(pow(pow(log(coord.x / coord.y), log(coord.y / coord.x)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float x = abs(coord.x);
		float y = abs(coord.y);
		coord = float2(x + pow(x, y) * cursor.x, y + pow(y, x) * cursor.y);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return float3(0, 0, 0); // black color within set
}

// Step Set

float3 stepSet(float2 coord, float2 cursor){
	uint i = 1; // iteration count

	while(((coord.y * (1.0 / coord.x)) - floor(coord.y * (1.0 / coord.x))) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		// coord = float2((coord.x * i) - floor(coord.x * i), ceil(coord.y * i) - (coord.y * i));
		coord *= i * distance(coord, cursor);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return float3(0, 0, 0); // black color within set
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

	float2 target;
	if(mode > 0) target = coords - cursor; 
	else target = float2(input.texcoord.x - 0.5, input.texcoord.y - 0.5) - cursor;

    if(abs(mode) >= 10 && abs(mode) < 20) return float4(juliaSet(target * FRACTAL_SIZE, cursor), 1.0f);
	else if(abs(mode) >= 20 && abs(mode) < 30) return float4(trigSet(target * FRACTAL_SIZE), 1.0f);
	else if(abs(mode) >= 30 && abs(mode) < 40) return float4(powerSet(target * FRACTAL_SIZE, cursorPos), 1.0f);
	else if(abs(mode) >= 40 && abs(mode) < 50) return float4(stepSet(target * FRACTAL_SIZE, cursorPos), 1.0f);
    else return float4(mandlebrotSet(target * FRACTAL_SIZE), 1.0f);
}