#define INCLUDE_BLOCK
#define INCLUDE_EXTBLOCK

#include "../Common.hlsl"

#include "../Vertex.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
	float effectSize;
	uint effectIters;
}

struct VS_OUTPUT { 
	float4 pos : SV_POSITION; 
	float3 texcoord : TEXCOORD0;
};

// Main

VS_OUTPUT main(VS_INPUT input, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	float4x4 cameraMatrix = getLookAtMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));
	output.texcoord = input.texcoord;

	if(instanceID > 0 && instanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[instanceID])) output.pos = mul(instanceData[instanceID], output.pos); // instanced transform

	return output;
}