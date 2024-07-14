#define INCLUDE_SCENEBLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float4 color;
	float3 offset;
	float3 rotation;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float4 vertex_pos : POSITION0;
	uint vertex_id : VERTEXID;
	float4 vertex_color : COLOR0;
};

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET {
	if(mode == 1) // directional mode
		return float4((input.vertex_pos.x / 2) + 0.5, (input.vertex_pos.y/ 2) + 0.5, (input.vertex_pos.z / 2) + 0.5, 1.0f);
	else if (mode == 2) // coordinate mode
		return float4((input.vertex_pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (input.vertex_pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (input.vertex_pos.z - offset.z) * 2.0 + 0.5, 1.0f);
	else if (mode == 3) // random color mode
		return getRandColor(color);
	else if (mode == 4) // vertex mode
		return input.vertex_color;
	else if (mode == -1) // indexing mode
		return getUniqueColor(input.vertex_id);
	else if(mode < -1){
		float fraction = float(input.vertex_id) / float(-mode);
		float level = 1.0 / float(-mode);

		/* uint i = 0;
		float4 outColor = float4(0.0, 0.0, 0.0, 0.0);
		for(float s = 0; s < 1.0; s += level){
			if(fraction - floor(fraction) < s) outColor = getUniqueColor(i);
			// else break;
			i++;
		} */
		// return outColor;

		if(fraction - floor(fraction) < 0.25) return float4(1.0, 1.0, 0.0, 0.8);
		if(fraction - floor(fraction) < 0.5) return float4(1.0, 0.0, 1.0, 0.8);
		if(fraction - floor(fraction) < 0.75) return float4(0.0, 1.0, 1.0, 0.8);
		else return float4(1.0, 1.0, 1.0, 0.8); // outColor;
	}
	else return color; // solid mode // default
}