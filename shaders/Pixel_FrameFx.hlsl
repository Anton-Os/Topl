struct PS_INPUT {
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	uint2 cursorPos : CURSOR;
};

float4 main(PS_INPUT input) : SV_TARGET{

	// Include effects here
	return float4(0.8f, 0.4f, 0.2f, 1.0f);
}