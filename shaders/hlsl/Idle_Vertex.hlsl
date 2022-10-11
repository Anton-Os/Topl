cbuffer CONST_BLOCK : register(b0) {
	uint actorID;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	uint model;
}

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT { float4 pos : SV_POSITION; };

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);

	return output;
}