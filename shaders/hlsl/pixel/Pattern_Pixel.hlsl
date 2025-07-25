#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#define PATTERN_SIZE 0.025

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
    float3 offset;
    float3 rotation;
    float3 scale;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

    double timeFrame;
	double timeElapse;
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 vertex_pos: POSITION1;
	float3 vertex_color : COLOR;
	float3 texcoord: TEXCOORD;
	float3 tangent: TANGENT;
};

// Functions

float4 trialPattern(float3 coords){
	uint m = uint(mode / 10) + 1;

	float r = abs(sin(coords.x * m) + cos(coords.y * m) + tan(coords.z * m));
	float g = abs(sin(coords.y * m) * cos(coords.z * m) * tan(coords.x * m));
	float b = abs(pow(pow(sin(coords.z * m), cos(coords.x * m)), tan(coords.y * m)));

	return float4(r, g, b, 1.0); 
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	// return float4(input.texcoord * input.vertex_color, 1.0);
	if(mode % 10 == 1) return trialPattern(input.vertex_pos);
	else if(mode % 10 == 2) return trialPattern(input.vertex_color);
	else if(mode % 10 == 3) return trialPattern(input.texcoord);
	else if(mode % 10 == 4) return trialPattern(input.tangent);
	else if(mode % 10 == 5) return trialPattern(input.vertex_pos - input.texcoord);
	else if(mode % 10 == 6) return trialPattern(input.tangent * input.vertex_color);
	else if(mode % 10 == 7) return trialPattern(input.vertex_pos / input.vertex_color);
	else if(mode % 10 == 8) return trialPattern(input.tangent + input.texcoord);
	else if(mode % 10 == 9) return trialPattern(float3(input.tangent.x, input.texcoord.y, input.vertex_pos.z));
	else return trialPattern(float3(input.pos.x, input.pos.y, input.pos.z));
}