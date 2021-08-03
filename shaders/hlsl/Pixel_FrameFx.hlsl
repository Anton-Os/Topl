struct PS_INPUT {
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	uint2 cursorPos : CURSOR;
};

float4 main(PS_INPUT input) : SV_TARGET{
	// Include effects here
	float2 pixelCoordsAdj = (input.pos.x / 1200, input.pos.y / 1100); // hard values

	uint mulFactor = 10;
	return float4(sin(pixelCoordsAdj.x * mulFactor), sin(pixelCoordsAdj.y * mulFactor), 1.0f, 1.0f);
}