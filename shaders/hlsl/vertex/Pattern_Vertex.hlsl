#define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.hlsl"

#include "Vertex.hlsl"

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

struct VS_OUTPUT { 
	float4 pos : SV_POSITION; 
	float3 vertex_pos: POSITION1;
	float3 vertex_color : COLOR;
	float3 texcoord: TEXCOORD;
	uint nearest_idx: INDEX0;
	uint second_idx : INDEX1; // second closest control point
	uint farthest_idx : INDEX2; // furthest distance control point
};

float3 getCtrlPoint(uint index){
	// float4 transformPoint = mul(ctrlMatrix, float4(target, 1.0));
	float3 ctrlPoint = (ctrlPoints[index] * scale) + offset;
	return mul(getRotMatrix(rotation), ctrlPoint);
}

uint calcNearestIndex(float3 target){ // TODO: Calculate with control matrix
	uint index = 0;
	for(uint n = 1; n < 8; n++) 
		if(length(target - getCtrlPoint(n)) < length(target - getCtrlPoint(index))) index = n;
	return index;
}

uint calcSecondIndex(float3 target){ // TODO: Calculate with control matrix
	uint nearIdx = calcNearestIndex(target);
	
	uint index = 0;
	if(index == nearIdx) index++; // avoids overriding closest point
	for(uint n = 1; n < 8; n++) 
		if(length(target - getCtrlPoint(n)) < length(target - getCtrlPoint(index)) && n != nearIdx) index = n;
	return index;
}

uint calcFarthestIndex(float3 target){ // TODO: Calculate with control matrix
	uint index = 0;
	for(uint n = 1; n < 8; n++) 
		if(length(target - getCtrlPoint(n)) > length(target - getCtrlPoint(index))) index = n;
	return index;
}

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output;

	float4 pos = getVertex(input.pos, offset, rotation, float4(scale, 1.0 / cam_pos.w));

	output.pos = mul(transpose(projMatrix), mul(getLookAtMatrix(cam_pos, look_pos), pos));
	output.vertex_pos = float3(output.pos.x, output.pos.y, output.pos.z);
	output.vertex_color = input.vert_color;
	output.texcoord = input.texcoord;
	output.nearest_idx = calcNearestIndex(output.vertex_pos);
	output.second_idx = calcSecondIndex(output.vertex_pos);
	output.farthest_idx = calcFarthestIndex(output.vertex_pos);
	// else output.vertex_color = // getRandColor(floor(distance(float4(output.nearestPoint, 1.0), output.pos) * 10));
#ifdef INCLUDE_EXTBLOCK
	if(instanceID > 0 && instanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[instanceID])) output.pos = mul(instanceData[instanceID], output.pos); // instanced transform
#endif
	return output;
}
