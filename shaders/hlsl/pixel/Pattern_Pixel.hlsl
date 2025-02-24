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
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 nearestPoint : POSITION;
	float3 vertex_color: COLOR;
};

// Functions

float4 colorPattern(float3 ctrlPoint, float3 pos, float3 color){
	float size = abs(mode) * PATTERN_SIZE;
	float distance = length(pos - ctrlPoint);

	if(mode > 0){
		ctrlPoint.x *= abs(sin(float(timeElapse) / 1000)) * (1.0 / size);
		ctrlPoint.y *= abs(cos(float(timeElapse) / 1000)) * (1.0 / size);
		ctrlPoint.z += abs(tan(float(timeElapse) / 1000)) * (1.0 / size);
	} else ctrlPoint /= (float(timeElapse) / 1000) * size;

	float r = ctrlPoint.x + abs(mode % 10) / distance;
	float g = ctrlPoint.y + abs(mode % 10) / distance;
	float b = ctrlPoint.z + abs(mode % 10) / distance;

	return float4(r, g, b, 1.0) * float4(color, 1.0);
}


// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

	return colorPattern(input.nearestPoint, input.pos, input.vertex_color);
}


