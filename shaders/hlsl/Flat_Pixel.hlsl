#define INCLUDE_SCENEBLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

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
	float3 texcoord: TEXCOORD;
};

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET {
	uint id = input.vertex_id;

	if (mode == -1) // indexing mode
		return getStepColor(primID);
	else if(mode < -1){
		uint target = input.vertex_id;
		while(target > uint(mode * -1)) target -= uint(mode * -1);
		return getStepColor(target);
	}
	else if(mode % 10 == 1) // directional mode
		return float4((input.pos.x / 2) + 0.5, (input.pos.y/ 2) + 0.5, (input.pos.z / 2) + 0.5, 1.0f);
	else if (mode % 10 == 2) // coordinate mode
		return float4((input.pos.x + cam_pos.x) * 2.0 + 0.5, (input.pos.y + cam_pos.y) * 2.0 + 0.5, input.pos.z * 2.0 + 0.5, 1.0f);
	else if (mode % 10 == 3) // vertex mode
		return input.vertex_color;
	else if (mode % 10 == 4) // camera mode
		return float4(abs(cam_pos.x - input.pos.x), abs(cam_pos.y - input.pos.y), abs(cam_pos.z - input.pos.z), 1.0);
	else if(mode % 10 == 5) // angular mode
		return float4(atan(input.vertex_pos.y / input.vertex_pos.z), atan(input.vertex_pos.z / input.vertex_pos.x), atan(input.vertex_pos.x / input.vertex_pos.y), 1.0);
	else if(mode % 10 == 6) // texcoord mode
		return float4(input.texcoord, 1.0);
	else if(mode % 10 == 7) // sectioned mode
		return float4(((primID + 1) / 9.0) - floor((primID + 1) / 9.0), ((primID + 1) / 6.0) - floor((primID + 1) / 6.0), ((primID + 1) / 3.0) - floor((primID + 1) / 3.0), 1.0);
	else if(mode % 10 == 8) // randomized mode
		return getRandColor(id);
	else if(mode % 10 == 9) // experimental mode
		return float4(dot(color, input.vertex_color), dot(float4(input.texcoord, 1.0), input.vertex_pos), dot(rotation, scale), 1.0);
	else return color; // solid mode // default
}