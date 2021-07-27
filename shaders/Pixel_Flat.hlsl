struct PS_INPUT {
	float4 pos : SV_POSITION;
	float4 flatColor : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET{
	// return float4(0.8f, 0.4f, 0.2f, 1.0f);
	return input.flatColor;
}