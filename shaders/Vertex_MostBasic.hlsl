cbuffer CONST_DISPLACE_BUFF : register(b0) {
	float3 offset;
}

struct VS_INPUT {
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) { // Only output is position
	VS_OUTPUT output;

	output.pos = float4(offset.x + input.pos.x, offset.y + input.pos.y, offset.z + input.pos.z, 1.0);
	output.texcoord = float2(input.texcoord[0], input.texcoord[1]);

	return output;
}

/* float4 main(VS_INPUT input): SV_POSITION{
	float4 pos = { offset.x, offset.y, offset.z, 0.0 };
	return input.pos + pos;
} */