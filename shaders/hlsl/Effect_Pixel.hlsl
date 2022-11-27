// Values

cbuffer CONST_BLOCK : register(b0) {
	uint renderID;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int2 screenRes;
	float2 cursorPos;
	uint mode;
}

struct PS_INPUT { float4 pos : SV_POSITION; };

// Functions

float3 cursorDist(float2 cursorPos, float2 pixelCoord){
	float red = 1.0f - (distance(cursorPos, pixelCoord) * 5); // receding color from center
	float green = (distance(cursorPos, pixelCoord) < 0.03) ? 1.0 : 0.0; // small acute bullseye center
	float blue = (distance(cursorPos, pixelCoord) - 0.1) / 5; // increasing color from center
	return float3(red, green, blue);
}

float3 whirlpools(float2 coord){
	float distances[9] = {
		distance(coord, float2(0.75, 0.75)), distance(coord, float2(0.25, 0.25)), distance(coord, float2(0.75, 0.25)), 
		distance(coord, float2(0.25, 0.75)), distance(coord, float2(0.5, 0.75)), distance(coord, float2(0.5, 0.25)),
		distance(coord, float2(0.75, 0.5)), distance(coord, float2(0.25, 0.5)), distance(coord, float2(0.5, 0.5))
	};

	float minDist = distances[0];
	for (uint m = 1; m < 9; m++) if (distances[m] < minDist) minDist = distances[m]; // calculate closest point

	return float3(sin(minDist * 10), cos(minDist * 100), tan(minDist * 500));
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	// effects go here
	float2 cursorPosAdj = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f;
	float2 pixelCoordsAdj = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y);

	if (mode == 1) return float4(whirlpools(pixelCoordsAdj), 1.0f); // special mode
	else return float4(cursorDist(cursorPosAdj, pixelCoordsAdj), 1.0f); // cursor mode // default
}