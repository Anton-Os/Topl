// Values

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
}

struct PS_INPUT { float4 pos : SV_POSITION; };

// Functions

float3 cursorTarget(float2 cursorPos, float2 coord){
	float red = (distance(cursorPos, coord) < 0.01) ? 1.0 : 0.0; // accute center
	float green = pow(1.0 - abs(cursorPos.x - coord.x), 5) - distance(cursorPos, coord); // x gradient
	float blue = pow(1.0 - abs(cursorPos.y - coord.y), 5) - distance(cursorPos, coord); // y gradient
	return float3(red, green, blue);
}

#define FRACTAL_SIZE 3.0 // max fractal size
#define FRACTAL_ITER 1000 // max fractal iteratons
#define C float2(cursorPos.x, cursorPos.y) // c value for julia set

// Julia set
float3 juliaSet(float2 coord){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = (coord.x * coord.x) - (coord.y * coord.y);
		double y = 2.0 * coord.x * coord.y;
		coord = float2(x, y) + C;
		i++;
	}

	if (i < FRACTAL_ITER) return float3(1.0f / i, tan(i), 0.05f * i); // custom colors outside set
	return float3(0, 0, 0); // black color within set
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float2 cursorPosAdj = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coordsAdj = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

	if (mode == 1) return float4(cursorTarget(cursorPosAdj, coordsAdj), 1.0f); // cursor track mode
	else return float4(juliaSet((coordsAdj - cursorPosAdj) * FRACTAL_SIZE), 1.0f); // fractal mode
}