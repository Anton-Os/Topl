#define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "../Common.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
    float3 offset;
    float3 rotation;
    float3 scale;

    float4x4 ctrlMatrix;
}

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

    double timeFrame;
	double timeElapse;
	float3 ctrlPoints[8];
	// float3 lightVal;
	// float3 lightPos;
}

struct VS_OUTPUT { 
	float4 pos : SV_POSITION; 
	float3 nearestPoint : POSITION1;	
	float3 vertex_color : COLOR;
};

float3 transformCtrlPoint(float3 target){
	float4 transformPoint = mul(ctrlMatrix, float4(target, 1.0));
	return float3(transformPoint.x, transformPoint.y, transformPoint.z);
}

float3 calcNearestPoint(float3 target){ // TODO: Calculate with control matrix
	float3 nearestPoint = transformCtrlPoint(ctrlPoints[0]);
	for(uint n = 1; n < 8; n++) 
		if(length(target - transformCtrlPoint(ctrlPoints[n])) < length(target - nearestPoint)) nearestPoint = ctrlPoints[n];
	return nearestPoint;
}

// Main

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	VS_OUTPUT output;

	float3 angles = mul(getRotMatrix(rotation), float3(input.pos.x, input.pos.y, input.pos.z));
	output.pos = float4(angles.x, angles.y, angles.z, 1.0) * float4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);
    // output.pos = output.pos * float4(1.0 + cos(timeElapse / 1000.0F), 1.0 + cos(timeElapse / 1000.0F), 1.0 + cos(timeElapse / 1000.0F), 1.0f);

	float4x4 cameraMatrix = getCamMatrix(cam_pos, look_pos);
	output.pos = mul(transpose(projMatrix), mul(cameraMatrix, output.pos + float4(offset, 0.0)));

	output.nearestPoint = calcNearestPoint(float3(output.pos.x, output.pos.y, output.pos.z));
	if(mode >= 0) output.vertex_color = input.vert_color;
	else output.vertex_color = getRandColor(floor(distance(float4(output.nearestPoint, 1.0), output.pos) * 10));

	return output;
}
