// Values

cbuffer CONST_BLOCK : register(b0) {
	uint renderID;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	uint mode;
}

struct PS_INPUT { float4 pos : SV_POSITION; };

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	return float4(1.0f, 1.0f, 0.0f, 1.0f); // yellow color test
}