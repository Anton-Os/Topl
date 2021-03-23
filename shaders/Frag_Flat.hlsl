struct PS_INPUT {
	float4 pos : SV_POSITION;
	uint flatColor : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET{
	return float4(0.8f, 0.4f, 0.2f, 1.0f); // TODO: Make this equal to flatColor input
}