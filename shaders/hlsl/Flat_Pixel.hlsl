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
	uint vertexNo : VERTEXID;
	float4 pos1 : POSITION0;
};

// Main

float4 main(PS_INPUT input) : SV_TARGET {
	if(mode == 1) { // alternate mode
		switch(input.vertexNo % 4){
			case 0: return float4(1.0f, 1.0f, 0.0f, 0.8f); break; // substract blue
			case 1: return float4(1.0f, 0.0f, 1.0f, 0.8f); break; // substract green
			case 2: return float4(0.0f, 1.0f, 1.0f, 0.8f); break; // substract red
			case 3: return float4(1.0f, 1.0f, 1.0f, 0.8f); break; // white
		}
	}
	else if (mode == 2) { // directional mode
		if (input.pos1.x > 0 && input.pos1.z > 0) return float4(1.0f, 1.0f, 0.0f, 0.8f);
		else if (input.pos1.x > 0 && input.pos1.z < 0) return float4(1.0f, 0.0f, 1.0f, 0.8f);
		else if (input.pos1.x < 0 && input.pos1.z > 0) return float4(0.0f, 1.0f, 1.0f, 0.8f);
		else if (input.pos1.x > 0 && input.pos1.z < 0) return float4(1.0f, 1.0f, 1.0f, 0.8f);
		else return float4(0.0f, 1.0f, 0.0f, 0.8f);
	}
	else return color; // solid mode // default
}