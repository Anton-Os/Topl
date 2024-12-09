#define INCLUDE_BLOCK
#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	double timeElapse;
}

struct PS_INPUT { float4 pos : SV_POSITION; };

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

    float r = sin(float(timeElapse) / 1000.0F);
	float g = cos(float(timeElapse) / 1000.0F);
	float b = tan(float(timeElapse) / 1000.0F);
	
	return float4(r, g, b, 1.0); // solid mode // default
}