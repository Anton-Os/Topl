#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#include "Common.hlsl"

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
};

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

	float nearestDist = length(float3(input.pos.x, input.pos.y, input.pos.z) - input.nearestPoint);

    float r = sin(float(timeElapse * input.pos.x * nearestDist) / 10000.0F);
	float g = cos(float(timeElapse * input.pos.y * nearestDist) / 10000.0F);
	float b = tan(float(timeElapse * input.pos.z * nearestDist) / 10000.0F);

	return float4(r, g, b, 1.0); // solid mode // default
}
