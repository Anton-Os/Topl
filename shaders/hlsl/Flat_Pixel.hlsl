// Values

cbuffer CONST_BLOCK : register(b0) {
	float4 color;
	float3 offset;
	float3 rotation;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	// float4 flatColor : COLOR0;
	float4 vertex_pos : POSITION0;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET {
	if(mode == 1) // view coordinate mode
		return float4((input.vertex_pos.x / 2) + 0.5, (input.vertex_pos.y/ 2) + 0.5, (input.vertex_pos.z / 2) + 0.5, 1.0f);
	else if (mode == 2) // actor coordinate mode
		return float4((input.vertex_pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (input.vertex_pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (input.vertex_pos.z - offset.z) * 2.0 + 0.5, 1.0f);
	else return color; // solid mode // default
}