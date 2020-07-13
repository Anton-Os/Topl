struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET {
	switch(input.texcoord[1]) {
		case(1.0f):
			return float4(0.8f, 0.3f, 0.4f, 1.0f);
		default:
			return float4(0.3f, 0.8f, 0.4f, 1.0f);
	}
}

/* float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(0.8f, 0.3f, 0.4f, 1.0f); // Random Color
} */