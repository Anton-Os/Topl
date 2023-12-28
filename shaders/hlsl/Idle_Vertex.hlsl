// Values

struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT { 
	float4 pos : SV_POSITION; 
	float4 color : COLOR0;
};

// Main

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;

	output.pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0);
	output.color = float4(1.0, 1.0, 0.0, 1.0); // yellow test

	return output;
}