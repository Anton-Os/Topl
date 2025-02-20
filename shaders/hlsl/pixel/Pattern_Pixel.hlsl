#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#define PATTERN_SIZE 0.5

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
    float3 offset;
    float3 rotation;
    float3 scale;

    float4x4 ctrlMatrix;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

    double timeFrame;
	double timeElapse;
	float3 ctrlPoints[8];
	// float3 lightVal;
	// float3 lightPos;
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 nearestPoint : POSITION;
	float3 vertex_color: COLOR;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

	float3 nearestPoint = float3(input.pos.x, input.pos.y, input.pos.z) - input.nearestPoint; 
	float nearestDist = length(float3(input.pos.x, input.pos.y, input.pos.z) - input.nearestPoint);
	float size = abs(mode) * PATTERN_SIZE;

    if(mode > 0){
		nearestPoint.x += sin(float(timeElapse) / 1000) * size;
		nearestPoint.y += cos(float(timeElapse) / 1000) * size;
		nearestPoint.z += tan(float(timeFrame) / 1000) * size;
	} else nearestPoint *= (float(timeElapse) / 1000) * size;

	float r = nearestPoint.x * abs(mode % 10) * nearestDist;
	float g = nearestPoint.y * abs(mode % 10) * nearestDist;
	float b = nearestPoint.z * abs(mode % 10) * nearestDist;

	// if(mode >= 0) return float4(r, g, b, 1.0) * float4(input.vertex_color, 1.0);
	return float4(r, g, b, 1.0) / float4(input.vertex_color, 1.0);
}


