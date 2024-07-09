#include "Common.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	// uint renderID;
	float3 offset;
	float3 rotation;
	float3 scale;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	float3 skyLight_pos; float3 skyLight_value;
	float3 flashLight_pos; float3 flashLight_value;
	float3 lampLight_pos; float3 lampLight_value;
}

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 vertex_pos : POSITION; // vertex position
	// float light_dist : LIGHT1 // light distance
	float3 light_pos : LIGHT1; // light position
	float3 light_val: LIGHT2; // light value
};

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	float4x4 cameraMatrix = getCamMatrix(cam_pos, look_pos);
	output.vertex_pos = float3(output.pos.x, output.pos.y, output.pos.z);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));
	output.light_pos = mul(transpose(projMatrix), mul(cameraMatrix, skyLight_pos));
	output.light_val = skyLight_value;

	return output;
}