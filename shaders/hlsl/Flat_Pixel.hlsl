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
	uint vertex_num : VERTEXID;
	float4 vertex_pos : POSITION0;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET {
	if(mode == 1) { // alternate mode
		switch(input.vertex_num % 5){
			case 0: return float4(0.5f, 0.5f, 1.0f, 0.8f);      
			case 1: return float4(0.0f, 0.0f, 1.0f, 0.8f); // substract blue
			case 2: return float4(1.0f, 0.0f, 1.0f, 0.8f); // substract green
			case 3: return float4(0.0f, 1.0f, 1.0f, 0.8f); // substract red
			case 4: return float4(1.0f, 1.0f, 1.0f, 0.8f); // white
			default: return float4(1.0f, 0.0f, 0.0f, color.a); // red
		}
	}
	else if (mode == 2) // directional mode
		return float4(input.vertex_pos.x - offset.x + 0.25, input.vertex_pos.y - offset.y + 0.25, input.vertex_pos.z - offset.z + 0.25, 0.8f);
	else return color; // solid mode // default
}