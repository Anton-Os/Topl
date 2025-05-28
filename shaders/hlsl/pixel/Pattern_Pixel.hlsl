#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#define PATTERN_SIZE 0.025

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
	float3 ctrlPoints[8];
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 nearestPoint : POSITION;
	float3 vertex_pos: POSITION1;
	float3 vertex_color: COLOR;
};

// Functions

float4 coordPattern(float3 ctrlPoint, float3 coords){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * PATTERN_SIZE;

	return float4(float3(dist * abs(ctrlPoint.r), dist * abs(ctrlPoint.g), dist * abs(ctrlPoint.b)) * size, 1.0);
}

float4 trigPattern(float3 ctrlPoint, float3 coords, float3 color){
	float distance = length(coords - ctrlPoint);
	float size = abs(mode % 100) * PATTERN_SIZE;

	ctrlPoint *= float3(abs(sin(float(timeElapse) / 1000)), abs(cos(float(timeElapse) / 1000)), abs(tan(float(timeElapse) / 1000))) * (1.0 / size);
	// else ctrlPoint /= (float(timeElapse) / 1000) * size;

	ctrlPoint += float3(abs(mode % 100) / distance, abs(mode % 100) / distance, abs(mode % 100) / distance);

	return float4(ctrlPoint * color, 1.0);
}

float4 centerPattern(float3 ctrlPoint, float3 coords){
	float3 relCoord = coords - ctrlPoint;
	float3 angles = float3(atan(relCoord.y / relCoord.x), atan(relCoord.x / relCoord.z), atan(relCoord.z / relCoord.y));

	return float4(ceil(angles.r) - angles.r, ceil(angles.g) - angles.g, ceil(angles.b) - angles.b, 1.0) * abs(mode % 100);
}

float4 proximaPattern(float3 ctrlPoint, float3 coords){
	float3 relCoord = coords - ctrlPoint;
	for(uint c = 0; c < 8; c++) 
		if(ctrlPoints[c].x != ctrlPoint.x && ctrlPoints[c].y != ctrlPoint.y && ctrlPoints[c].z != ctrlPoint.z)
			relCoord += (coords - ctrlPoint) / abs(mode % 100);

	return float4(relCoord, 1.0);
}

float4 neonPattern(uint ctrlIdx, float3 coords){
	float3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = pow(abs(relCoord.x), abs(relCoord.y));
	float g = pow(abs(relCoord.x), abs(relCoord.z));
	float b = pow(abs(relCoord.z), abs(relCoord.y));

	return float4((relCoord * float3(r, g, b)) / abs(mode % 100), 1.0);
}

float4 gradPattern(uint ctrlIdx, float3 coords){
	float3 relCoord = ctrlPoints[ctrlIdx] - coords;
	for(uint c = 0; c < 8; c++) if(c != ctrlIdx) relCoord += (ctrlPoints[c] - coords) * abs(cos(c));

	// return float4(relCoord * ((timeElapse * 5) - floor(timeElapse * 5)), 1.0);
	return float4(relCoord, 1.0);
}

float4 crossPattern(uint ctrlIdx, float3 coords){
	float3 coord1 = ctrlPoints[ctrlIdx] - coords;
	float3 coord2 = ctrlPoints[8 - ctrlIdx - 1] + coords;

	return float4(cross(coord1, coord2), 1.0);
}

float4 flowPattern(uint ctrlIdx, float3 coords){
	float3 nearestPoint = ctrlPoints[ctrlIdx] - coords;
	float3 secondPoint = float3(0.0, 0.0, 0.0);
	for(uint c = 0; c < 8; c++)
		if(c != ctrlIdx && distance(ctrlPoints[c] - coords, nearestPoint) < distance(secondPoint, nearestPoint))
			secondPoint = ctrlPoints[c] - coords;

	if(distance(nearestPoint, secondPoint) < 0.1) return float4(coords, 1.0);
	else return float4((nearestPoint - secondPoint) * distance(nearestPoint, coords) * 5, 1.0);
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

	float3 target;
	if(mode >= 0) target = input.vertex_pos;
	else target = input.pos;
	
	float3 relCoord = input.nearestPoint - target;
	int nearestIdx = -1;
	for(uint c = 0; c < 8; c++) 
		if(ctrlPoints[c].x == input.nearestPoint.x && ctrlPoints[c].y == input.nearestPoint.y && ctrlPoints[c].z == input.nearestPoint.z) 
			nearestIdx = c;

	if(abs(mode) > 0 && abs(mode) < 100) return  coordPattern(input.nearestPoint, target);
	else if(abs(mode) >= 100 && abs(mode) < 200) return trigPattern(input.nearestPoint, target, input.vertex_color);
	else if(abs(mode) >= 200 && abs(mode) < 300) return centerPattern(input.nearestPoint, target);
	else if(abs(mode) >= 300 && abs(mode) < 400) return proximaPattern(input.nearestPoint, target);
	else if(abs(mode) >= 400 && abs(mode) < 500 && nearestIdx > -1) return neonPattern(nearestIdx, target);
	else if(abs(mode) >= 500 && abs(mode) < 600 && nearestIdx > -1) return gradPattern(nearestIdx, target);
	else if(abs(mode) >= 600 && abs(mode) < 700 && nearestIdx > -1) return crossPattern(nearestIdx, target);
	else if(abs(mode) >= 700 && abs(mode) < 800 && nearestIdx > -1) return flowPattern(nearestIdx, target);
	else if(abs(mode) >= 800 && abs(mode) < 900) return float4(pow(relCoord.x, relCoord.y), pow(relCoord.y, relCoord.z), pow(relCoord.z, relCoord.x), 1.0);
	else return float4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);
}


