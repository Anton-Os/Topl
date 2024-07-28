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
};

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET {
	uint id = input.vertex_id;

	if(mode < -1){
		uint target = input.vertex_id;
		while(target > uint(mode * -1)) target -= uint(mode * -1);
		return getUniqueColor(target);
	}
	if(mode == 1) // directional mode
		return float4((input.vertex_pos.x / 2) + 0.5, (input.vertex_pos.y/ 2) + 0.5, (input.vertex_pos.z / 2) + 0.5, 1.0f);
	else if (mode == 2) // coordinate mode
		return float4((input.vertex_pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (input.vertex_pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (input.vertex_pos.z - offset.z) * 2.0 + 0.5, 1.0f);
	else if (mode == 3) // random color mode
		return getRandColor(color);
	else if (mode == 4) // vertex mode
		return input.vertex_color;
	else if (mode == 5) // scale mode
	 	return color * float4(scale.x / (input.vertex_pos.x - offset.x), scale.y / (input.vertex_pos.y - offset.y), scale.z / (input.vertex_pos.z - offset.z), 1.0);
	else if(mode == 6) // id mode
		return float4(((id + 1) / 3.0) - floor((id + 1) / 3.0), ((id + 1) / 6.0) - floor((id + 1) / 6.0), ((id + 1) / 9.0) - floor((id + 1) / 9.0), 1.0);
	else if(mode == 7) // camera mode
		return float4(abs(cam_pos.x - offset.x), abs(cam_pos.y - offset.y), abs(cam_pos.z - offset.z), 1.0);
	else if(mode == 8) // dope mode
		return float4(sin(input.vertex_color.r * id), cos(color.g * primID), tan(color.b * input.vertex_color.b * id * primID), 1.0);
	else if (mode == -1) // indexing mode
		return getUniqueColor(input.vertex_id);
	else return color; // solid mode // default
}