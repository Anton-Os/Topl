#define INCLUDE_BLOCK
#define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

    double timeFrame;
	double timeElapse;
	float2 ctrlPoints[8];
	// float3 lightVal;
	// float3 lightPos;
}

struct VS_OUTPUT { 
	float4 pos : SV_POSITION; 
	float2 nearestPoint : POSITION1;	
};

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);
    // output.pos = output.pos * float4(1.0 + cos(timeElapse / 1000.0F), 1.0 + cos(timeElapse / 1000.0F), 1.0 + cos(timeElapse / 1000.0F), 1.0f);

	float4x4 cameraMatrix = getCamMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));

	float2 nearestPoint = ctrlPoints[0];
	for(uint n = 1; n < 8; n++) 
		if(length(float2(output.pos.x, output.pos.y) - ctrlPoints[n]) < length(float2(output.pos.x, output.pos.y) - nearestPoint)) nearestPoint = ctrlPoints[n];
	output.nearestPoint = nearestPoint;

	return output;
}
