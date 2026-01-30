#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#define FIELD_SIZE 0.025

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
    float3 offset;
    float3 rotation;
    float3 scale;

    float4x4 ctrlMatrix;
	float alpha;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

    double timeFrame;
	double timeElapse;
	float3 ctrlPoints[16];
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 vertex_pos: POSITION1;
	float3 vertex_color : COLOR;
	float3 texcoord: TEXCOORD;
	float3 tangent: TANGENT;
	uint nearest_idx: INDEX0;
	uint second_idx : INDEX1; // second closest control point
	uint farthest_idx : INDEX2; // furthest distance control point
};

#include "field/Field.hlsl"

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float4 outColor;

	uint m = abs(mode) % 1000;
	if(timeElapse == 0.0) outColor = float4(1.0, 1.0, 1.0, 0.75); // test

	float3 target = input.vertex_pos;
	// if(mode < 0) target = input.vertex_color;
	if(mode < 0) target = input.tangent; // testing

	uint ctrlIdx = input.nearest_idx;
	if(mode % 3 == 1) ctrlIdx = input.second_idx;
	else if(mode % 3 == 2) ctrlIdx = input.farthest_idx;
	
	float3 nearestPoint = ctrlPoints[ctrlIdx];

	/* if(mode >= 1000){
		float2 screenCoords = float2(input.pos.x / (floor(mode / 1000) * 1000.0), input.pos.y / (floor(mode / 1000) * 1000.0)); // adjusted coordinates
		screenCoords = (screenCoords - float2(0.5, 0.5)) * 2.0;

		nearestPoint *= sin(timeElapse / 1000) * floor(mode / 1000);
		target *= float3(sin(screenCoords.x * (mode % 100)), cos(screenCoords.y * (mode % 100)), tan(screenCoords.x * screenCoords.y * (mode % 100)));
	} */

	float3 relCoord = nearestPoint - target;

	if(m > 0 && m < 100) outColor = coordField(nearestPoint, target);
	else if(m >= 100 && m < 200) outColor = trigField(nearestPoint, target, input.vertex_color);
	else if(m >= 200 && m < 300) outColor = centerField(nearestPoint, target);
	else if(m >= 300 && m < 400) outColor = proximaField(nearestPoint, target);
	else if(m >= 400 && m < 500) outColor = neonField(ctrlIdx, target);
	else if(m >= 500 && m < 600) outColor = gradField(ctrlIdx, target);
	else if(m >= 600 && m < 700) outColor = crossField(ctrlIdx, target);
	else if(m >= 700 && m < 800) outColor = flowField(ctrlIdx, target);
	else if(m >= 800 && m < 900) outColor = modalTex(ctrlIdx, target);
	// else if(m >= 800 && m < 900) outColor = float4(pow(relCoord.x, relCoord.y), pow(relCoord.y, relCoord.z), pow(relCoord.z, relCoord.x), 1.0);
	else outColor = float4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);

	return float4(abs(outColor.r), abs(outColor.g), abs(outColor.b), outColor.a);
}