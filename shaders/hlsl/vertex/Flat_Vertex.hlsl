#define INCLUDE_EXTBLOCK
#define INCLUDE_SCENEBLOCK

#include "../Common.hlsl"

#include "../Vertex.hlsl"

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
	float3 texcoord : TEXCOORD;
};

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float4 pos = getVertex(input.pos, offset, rotation, float4(scale, 1.0 / cam_pos.w));

	output.vertex_pos = pos;
	output.pos = mul(transpose(projMatrix), mul(getLookAtMatrix(cam_pos, look_pos), pos));
	output.vertex_id = vertexID;
	output.texcoord = input.texcoord;
	if(mode < 10 || vertexID % mode == 0) output.vertex_color = float4(input.vert_color, 1.0);
	else output.vertex_color = float4(0.0F, 0.0f, 0.0F, 0.5F);
#ifdef INCLUDE_EXTBLOCK
	if(instanceID > 0 && instanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[instanceID])) output.pos = mul(instanceData[instanceID], output.pos); // instanced transform
#endif
	return output;
}