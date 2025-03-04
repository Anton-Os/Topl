#define INCLUDE_SCENEBLOCK
#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS

#define FLAT_ID 0
#define FLAT_DIRECTIONAL 1
#define FLAT_COORD 2
#define FLAT_VERTEX 3
#define FLAT_CAMERA 4
#define FLAT_ANGULAR 5
#define FLAT_TEXCOORD 6
#define FLAT_NORMAL 7
#define FLAT_SECTIONED 8
#define FLAT_RANDOM 9
// #define FLAT_TRIAL 9

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float4 color;
	float3 offset;
	float3 rotation;
	float3 scale;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float4 vertex_pos : POSITION0;
	uint vertex_id : VERTEXID;
	float4 vertex_color : COLOR0;
	float3 normal : NORMAL;
	float3 texcoord: TEXCOORD;
};

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET {
	uint id = input.vertex_id;
	
	float alpha = 1.0f;
	if(abs(mode) >= 10) alpha -= -input.pos.z * ceil(mode / 10.0);

	if(mode >= 10) return input.vertex_color;
	else if(mode == -1) return getStepColor(primID); // indexing mode
	else if(mode < -1){
		uint target = input.vertex_id;
		while(target > uint(mode * -1)) target -= uint(mode * -1);
		return getStepColor(target);
	}
	else if(mode % 10 == FLAT_DIRECTIONAL) return float4((input.pos.x / 2.0) + 0.5, (input.pos.y / 2.0) + 0.5, (input.pos.z / 2.0) + 0.5, alpha); // Troubleshoot this!
	else if(mode % 10 == FLAT_COORD) return float4((input.vertex_pos.x + cam_pos.x) * 2.0 + 0.5, (input.vertex_pos.y + cam_pos.y) * 2.0 + 0.5, input.vertex_pos.z * 2.0 + 0.5, alpha);
	else if(mode % 10 == FLAT_VERTEX) return input.vertex_color;
	else if(mode % 10 == FLAT_CAMERA) return float4(abs(cam_pos.x - input.pos.x), abs(cam_pos.y - input.pos.y), abs(cam_pos.z - input.pos.z), alpha);
	else if(mode % 10 == FLAT_ANGULAR) return float4(atan(input.vertex_pos.y / input.vertex_pos.z), atan(input.vertex_pos.z / input.vertex_pos.x), atan(input.vertex_pos.x / input.vertex_pos.y), alpha);
	else if(mode % 10 == FLAT_TEXCOORD) return float4(input.texcoord, alpha);
	else if(mode % 10 == FLAT_NORMAL) return float4(input.normal, alpha);
	else if(mode % 10 == FLAT_SECTIONED) return float4(((primID + 1) / 9.0) - floor((primID + 1) / 9.0), ((primID + 1) / 6.0) - floor((primID + 1) / 6.0), ((primID + 1) / 3.0) - floor((primID + 1) / 3.0), alpha);
	else if(mode % 10 == FLAT_RANDOM) return float4(getRandColor(floor((color.r * color.g * color.b) * 995.49)), alpha);
	// else if(mode % 10 == FLAT_TRIAL) return float4(dot(color, input.vertex_color), dot(float4(input.texcoord, 1.0), input.vertex_pos), dot(rotation, scale), alpha);
	else return color; // solid mode // default
}