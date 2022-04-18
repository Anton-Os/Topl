struct PS_INPUT {
	float4 pos : SV_POSITION;
	uint2 screenRes : RESOLUTION;
	float2 cursorPos : CURSOR;
	uint mode : MODE;
	uint actorID : ACTOR;
};

/* float3 quadTest(float2 pixelCoord){
	if(pixelCoord.x > 0.5 & pixelCoord.y > 0.0) return float3(1.0f, 0.0f, 0.0f);
	else if(pixelCoord.x < 0.5 & pixelCoord.y > 0.0) return float3(0.0f, 1.0f, 0.0f);
	else if(pixelCoord.x < 0.5 & pixelCoord.y < 0.0) return float3(0.0f, 0.0f, 1.0f);
	else return float3(0.0f, 0.0f, 0.0f);
} */

float3 cursorDist(float2 cursorPos, float2 pixelCoord){
	float red = 1.0f - (distance(cursorPos, pixelCoord) * 5); // receding color from center
	float green = (distance(cursorPos, pixelCoord) < 0.03) ? 1.0 : 0.0; // small acute bullseye center
	float blue = (distance(cursorPos, pixelCoord) - 0.1) / 5; // increasing color from center
	return float3(red, green, blue);
}

float3 mandlebrot(uint2 screenRes, float2 pixelCoord){
	const double size = 10.0f;
	const uint max_iters = 1000;
	
	double x = 0; double y = 0;
	uint iter = 0;
	while(x * x + y * y <= size && iter < max_iters){
		double temp = (x * x) - (y * y) + pixelCoord.x;
		y = (2 * x * y) + pixelCoord.y;
		x = temp;
		iter++;
	}

	// Black and White
	if(iter < max_iters) return float3(1.0f, 1.0f, 1.0f); // white pixel
	else return float3(0.0f, 0.0f, 0.0f); // black pixel
}


float3 mandlebulb(uint2 screenRes, float2 pixelCoord){
	return float3(0.0f, 0.0f, 1.0f); // placeholder
}

float4 main(PS_INPUT input) : SV_TARGET{
	// Include effects here
	float2 pixelCoordsAdj = float2(input.pos.x / input.screenRes.x, input.pos.y / input.screenRes.y); 
	float2 pixelOffset = float2(-0.25f, -0.25f);

	if(input.mode == 0) return float4(cursorDist(input.cursorPos, pixelCoordsAdj), 1.0f); // cursor mode
	else if (input.mode == 1) return float4(mandlebrot(input.screenRes, pixelCoordsAdj + pixelOffset), 1.0f); // fractal mode
	else return float4(1.0f, 0.0f, 0.0f, 1.0f); // mode not supported
	// return float4(quadTest(pixelCoordsAdj), 1.0f);
}