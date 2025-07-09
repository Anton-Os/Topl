#define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.hlsl"

#include "Vertex.hlsl"

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

struct VS_OUTPUT { 
	float4 pos : SV_POSITION; 
	float3 vertex_pos: POSITION1;
	float3 vertex_color : COLOR;
	float3 texcoord: TEXCOORD;
	float3 tangent: TANGENT;
};

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float4 pos = getVertex(input.pos, offset, rotation, float4(scale, 1.0 / cam_pos.w));

	output.pos = mul(transpose(projMatrix), mul(getLookAtMatrix(cam_pos, look_pos), pos));
	output.vertex_pos = float3(output.pos.x, output.pos.y, output.pos.z);
	output.vertex_color = input.vert_color;
	output.texcoord = input.texcoord;
	output.tangent = input.tangent;
#ifdef INCLUDE_EXTBLOCK
	if(instanceID > 0 && instanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[instanceID])) output.pos = mul(instanceData[instanceID], output.pos); // instanced transform
#endif
	return output;
}
