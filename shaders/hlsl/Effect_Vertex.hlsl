// Values

cbuffer CONST_BLOCK : register(b0) {
	uint actorID;
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

struct VS_OUTPUT {
	uint actorID : ACTOR;
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	float2 cursorPos : CURSOR;
	uint mode: MODE;
};

// Main

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	float4 final_pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);
	float2 cursorPosAdj = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f;

	output.pos = final_pos;
	output.screenRes = screenRes;
	output.cursorPos = cursorPosAdj; // adjusted coordinates
	output.mode = mode;
	output.actorID = actorID;

	return output;
}