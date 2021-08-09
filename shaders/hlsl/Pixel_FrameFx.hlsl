struct PS_INPUT {
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	float2 cursorPos : CURSOR;
};

float4 main(PS_INPUT input) : SV_TARGET{
	// Include effects here
	float2 pixelCoordsAdj = (input.pos.x / input.screenRes.x, input.pos.y / input.screenRes.y); // hard values

	uint m = 10;
	if(input.cursorPos.x > 0.0)
		return float4(sin(pixelCoordsAdj.x * m), sin(pixelCoordsAdj.y * m), 1.0f, 1.0f);
	else
		return float4(cos(pixelCoordsAdj.x * m), cos(pixelCoordsAdj.y * m), 1.0f, 1.0f);
}