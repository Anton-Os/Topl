#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
	float effectSize;
	uint effectIters;
}

#define FRACTAL_SIZE 10.0
#define FRACTAL_ITER 15

struct PS_INPUT { 
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

#include "effect/Effect.hlsl"

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	uint m = abs(mode) % 100;
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

	float2 target = coords - cursor;
	if(mode >= 0) target = coords - cursor; 
	else target = float2(input.texcoord.x, input.texcoord.y) - cursor;
	target += float2(cam_pos.x, cam_pos.y);
	target *= FRACTAL_SIZE;

    if(m >= 10 && m < 20) return juliaSet(target, cursor);
	else if(m >= 20 && m < 30) return trigSet(target);
	else if(m >= 30 && m < 40) return powerSet(target, cursorPos);
    else if(m >= 40 && m < 50) return wingSet(target);
    else if(m >= 50 && m < 60) return stepSet(target, cursorPos);
	else if(m >= 60 && m < 70) return loopSet(target);
	else if(m >= 70 && m < 80) return shardSet(target, cursorPos);
	else if(m >= 80 && m < 90) return sparseSet(target, distance(coords, cursor));
	else if(m >= 90 && m < 100) return retroSet(target, cursor);
	// else if(abs(mode) >= 100 && abs(mode) < 110) return juliaSet(float2(mandlebrotSet(target).r, mandlebrotSet(target).g), cursor);
	// else if(abs(mode) >= 110 && abs(mode) < 120) return trigSet(float2(tan(powerSet(target, cursor).r), 1.0 / tan(powerSet(target, cursor).g)));
	// else if(abs(mode) >= 120 && abs(mode) < 130) return stepSet(float2(wingSet(target).r * wingSet(target).b, wingSet(target).g / wingSet(target).b), cursor);
    else return mandlebrotSet(target);
}
