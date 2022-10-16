// Values

cbuffer CONST_BLOCK : register(b0) {
	uint renderID;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int2 screenRes;
	float2 cursorPos;
	uint mode;
}

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT { float4 pos : SV_POSITION; };

// Main

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);

	return output;
}