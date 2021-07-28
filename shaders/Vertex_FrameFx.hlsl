cbuffer CONST_BLOCK : register(b0) {
	uint renderID;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	uint2 screenRes;
	uint2 cursorPos;
}

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	uint2 cursorPos : CURSOR;
};

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	float4 finalPos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);

	output.screenRes = screenRes;
	output.cursorPos = cursorPos;
	output.pos = finalPos;

	return output;
}