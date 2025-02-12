#define INCLUDE_BLOCK

#include "../Common.hlsl"

#include "Vertex.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	float3 lightPos;
	float3 lightVal;
}

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 vertex_pos : POSITION; // vertex position
	float3 normal : NORMAL;
	float3 texcoord: TEXCOORD;
};

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	output.vertex_pos = float3(output.pos.x, output.pos.y, output.pos.z);
	output.normal = input.normal;
	output.pos = mul(transpose(projMatrix), mul(getCamMatrix(cam_pos, look_pos), output.pos + float4(offset, 0.0)));
	output.texcoord = input.texcoord;

	return output;
}