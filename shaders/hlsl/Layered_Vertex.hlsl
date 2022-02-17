cbuffer CONST_BLOCK : register(b0) {
	float4 offset;
	float4 rotation;
}

/* cbuffer CONST_SCENE_BLOCK : register(b1) {
	float4x4 projMatrix;
} */

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);

	output.pos += offset;
	output.texcoord = float2(input.texcoord.u, input.texcoord.v);

	return output;
}