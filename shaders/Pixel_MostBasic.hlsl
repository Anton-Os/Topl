struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET{
	if (input.texcoord[0] < 0.5)
		return float4(0.8f, 0.3f, 0.4f, 1.0f);
	else
		return float4(0.3f, 0.8f, 0.4f, 1.0f);
}

/* float4 main() : SV_TARGET
{
	return float4(0.8f, 0.3f, 0.4f, 1.0f); // Random Color
} */