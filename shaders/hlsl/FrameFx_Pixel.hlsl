struct PS_INPUT {
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	float2 cursorPos : CURSOR;
};

float3 mandlebrot(float2 fragCoord){
	const float origin_len = 1.0f;
	const float max_steps = 1000.0f;
	float steps = 0.0f;

	// TODO: Add set generation logic

	float f = steps / max_steps;
	return float3(f, f, f); // should return black color without any further calculations
}

float3 cursorDist(float2 cursorPos, float2 fragCoord){
	float red = 1.0f - (distance(cursorPos, fragCoord) * 5); // receding color from center
	float green = (distance(cursorPos, fragCoord) < 0.03) ? 1.0 : 0.0; // small acute bullseye center
	float blue = (distance(cursorPos, fragCoord) - 0.1) / 5; // increasing color from center
	return float3(red, green, blue);
}

float3 colorQuad(float2 fragCoord){
	if(fragCoord.x > 0.5 & fragCoord.y > 0.0) return float3(1.0f, 0.0f, 0.0f);
	else if(fragCoord.x < 0.5 & fragCoord.y > 0.0) return float3(0.0f, 1.0f, 0.0f);
	else if(fragCoord.x < 0.5 & fragCoord.y < 0.0) return float3(0.0f, 0.0f, 1.0f);
	else return float3(0.0f, 0.0f, 0.0f);
}

float4 main(PS_INPUT input) : SV_TARGET{
	// Include effects here
	float2 pixelCoordsAdj = float2(input.pos.x / input.screenRes.x, input.pos.y / input.screenRes.y); 

	return float4(cursorDist(input.cursorPos, pixelCoordsAdj), 1.0f);
	// return float4(colorQuad(pixelCoordsAdj), 1.0f);
	// return float4(pixelCoordsAdj.x, pixelCoordsAdj.y, 0.0, 1.0);
}