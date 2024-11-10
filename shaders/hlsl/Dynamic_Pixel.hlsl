#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	double timeElapse;
	// TODO: Include other dynamic parameters
}

struct PS_INPUT { float4 pos : SV_POSITION; };

// Main

float4 main(PS_INPUT input) : SV_TARGET {
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 1.0);
	
	double r = sin(timeElapse / 1000.0);
	double g = cos(timeElapse / 1000.0);
	double b = tan(timeElapse / 1000.0);

	return float4(r, g, b, 1.0);
}