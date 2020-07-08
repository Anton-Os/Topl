cbuffer CONST_DISPLACE_BUFF : register(b0) {
	float3 offset;
}

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(VS_INPUT input) : SV_POSITION{ // Only output is position
	float4 pos = { offset.x, offset.y, offset.z, 0.0 };
	return input.pos + pos;
}