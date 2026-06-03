#define INCLUDE_BLOCK
#define INCLUDE_EXTBLOCK

#include "Common.hlsl"

#include "Vertex.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
	// double drawSize;
	float4 tracerSteps[TRACER_STEPS / 2]; // TODO: This needs to be packed
	float4 tracerPaths[TRACER_STEPS / 2]; // TODO: This needs to be packed
}

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Main

VS_OUTPUT main(VS_INPUT input, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float4 pos = getVertexInstance(input.pos, offset, rotation, float4(scale, 1.0 / cam_pos.w), instanceID);

	float4x4 cameraMatrix = getLookAtMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(getLookAtMatrix(cam_pos, look_pos), pos));
	output.texcoord = input.texcoord;

	return output;
}