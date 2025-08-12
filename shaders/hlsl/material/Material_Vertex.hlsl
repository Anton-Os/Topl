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

	float3 texScroll; // texture coordinate scrolling
	float4 texScale; // texture coordinate scaling

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

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float4 pos = getVertex(input.pos, offset, rotation, float4(scale, 1.0 / cam_pos.w));

	output.vertex_pos = float3(output.pos.x, output.pos.y, output.pos.z);
	output.normal = input.normal;
	output.pos = mul(transpose(projMatrix), mul(getLookAtMatrix(cam_pos, look_pos), pos));
	output.texcoord = (input.texcoord - texScroll) * float3(texScale.x, texScale.y, texScale.z);
#ifdef INCLUDE_EXTBLOCK
	if(instanceID > 0 && instanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[instanceID])) output.pos = mul(instanceData[instanceID], output.pos); // instanced transform
#endif
	return output;
}