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

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(timeElapse == 0.0) return float4(1.0, 1.0, 1.0, 0.75); // test

	float3 target;
	if(mode >= 0) target = input.vertex_pos;
	else target = input.pos;
	
	float3 relCoord = input.nearestPoint - target;

	if(abs(mode) > 0 && abs(mode) < 100) return  coordPattern(input.nearestPoint, target);
	else if(abs(mode) >= 100 && abs(mode) < 200) return trigPattern(input.nearestPoint, target, input.vertex_color);
	else if(abs(mode) >= 200 && abs(mode) < 300) return centerPattern(input.nearestPoint, target);
	else if(abs(mode) >= 300 && abs(mode) < 400) return proximaPattern(input.nearestPoint, target);
	else return float4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);
}


