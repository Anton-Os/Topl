#define INCLUDE_SCENEBLOCK

#include "Common.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float4 color;
	float3 offset;
	float3 rotation;
	float3 scale;
}

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float4 vertex_pos : POSITION0;
	uint vertex_id : VERTEXID;
	float4 vertex_color : COLOR0;
};

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) { // Only output is position
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	float4x4 cameraMatrix = getCamMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));
	output.vertex_pos = output.pos;
	output.vertex_id = vertexID;
	output.vertex_color = color - (color / (float)(vertexID + 1)); // getUniqueColor(vertexID);

	return output;
}