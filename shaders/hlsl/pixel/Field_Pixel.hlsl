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

// Functions

float4 coordField(float3 ctrlPoint, float3 coords){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * FIELD_SIZE;

	return float4(float3(dist * abs(ctrlPoint.r), dist * abs(ctrlPoint.g), dist * abs(ctrlPoint.b)) * size, 1.0);
}

float4 trigField(float3 ctrlPoint, float3 coords, float3 color){
	float distance = length(coords - ctrlPoint);
	float size = abs(mode % 100) * FIELD_SIZE;

	ctrlPoint *= float3(abs(sin(float(timeElapse) / 1000)), abs(cos(float(timeElapse) / 1000)), abs(tan(float(timeElapse) / 1000))) * (1.0 / size);
	// else ctrlPoint /= (float(timeElapse) / 1000) * size;

	ctrlPoint += float3(abs(mode % 100) / distance, abs(mode % 100) / distance, abs(mode % 100) / distance);

	return float4(ctrlPoint * color, 1.0);
}

float4 centerField(float3 ctrlPoint, float3 coords){
	float3 relCoord = coords - ctrlPoint;
	float3 angles = float3(atan(relCoord.y / relCoord.x), atan(relCoord.x / relCoord.z), atan(relCoord.z / relCoord.y));

	return float4(ceil(angles.r) - angles.r, ceil(angles.g) - angles.g, ceil(angles.b) - angles.b, 1.0) * abs(mode % 100);
}

float4 proximaField(float3 ctrlPoint, float3 coords){
	float3 relCoord = coords - ctrlPoint;
	for(uint c = 0; c < 8; c++) 
		if(ctrlPoints[c].x != ctrlPoint.x && ctrlPoints[c].y != ctrlPoint.y && ctrlPoints[c].z != ctrlPoint.z)
			relCoord += (coords - ctrlPoint) / abs(mode % 100);

	return float4(relCoord, 1.0);
}

float4 neonField(uint ctrlIdx, float3 coords){
	float3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = pow(abs(relCoord.x), abs(relCoord.y));
	float g = pow(abs(relCoord.x), abs(relCoord.z));
	float b = pow(abs(relCoord.z), abs(relCoord.y));

	return float4((relCoord * float3(r, g, b)) / abs(mode % 100), 1.0);
}

float4 gradField(uint ctrlIdx, float3 coords){
	float3 relCoord = ctrlPoints[ctrlIdx] - coords;
	for(uint c = 0; c < 8; c++) if(c != ctrlIdx) relCoord += (ctrlPoints[c] - coords) * abs(cos(pow(c, (abs(mode) % 100) + 1)));

	// return float4(relCoord * ((timeElapse * 5) - floor(timeElapse * 5)), 1.0);
	return float4(relCoord, 1.0);
}

float4 crossField(uint ctrlIdx, float3 coords){
	float3 coord1 = ctrlPoints[ctrlIdx] - coords;
	float3 coord2 = ctrlPoints[8 - ctrlIdx - 1] + coords;

	return float4(cross(coord1, coord2) / ((abs(mode) % 100) + 1), 1.0);
}

float4 flowField(uint ctrlIdx, float3 coords){
	float3 nearestPoint = ctrlPoints[ctrlIdx] - coords;
	float3 secondPoint = float3(10000.0, 10000.0, 10000.0);
	for(uint c = 0; c < 8; c++)
		if(c != ctrlIdx && distance(ctrlPoints[c] - coords, nearestPoint) < distance(secondPoint, nearestPoint))
			secondPoint = ctrlPoints[c] - coords;

	if(abs(length(nearestPoint - secondPoint)) < length(coords))
		return float4((nearestPoint - secondPoint) * distance(nearestPoint, coords) * ((abs(mode) % 100) + 1), 1.0);
	else return float4(1.0, 1.0, 1.0, 0.15);
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	uint m = abs(mode) % 1000;
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

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

	if(m > 0 && m < 100) return coordField(nearestPoint, target);
	else if(m >= 100 && m < 200) return trigField(nearestPoint, target, input.vertex_color);
	else if(m >= 200 && m < 300) return centerField(nearestPoint, target);
	else if(m >= 300 && m < 400) return proximaField(nearestPoint, target);
	else if(m >= 400 && m < 500) return neonField(ctrlIdx, target);
	else if(m >= 500 && m < 600) return gradField(ctrlIdx, target);
	else if(m >= 600 && m < 700) return crossField(ctrlIdx, target);
	else if(m >= 700 && m < 800) return flowField(ctrlIdx, target);
	else if(m >= 800 && m < 900) return modalTex(ctrlIdx, target);
	// else if(m >= 800 && m < 900) return float4(pow(relCoord.x, relCoord.y), pow(relCoord.y, relCoord.z), pow(relCoord.z, relCoord.x), 1.0);
	else return float4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);
}