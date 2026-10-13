#define INCLUDE_MESHBLOCK
#define IGNORE_INPUTS

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
	uint near_idx: INDEX0;
	uint sec_idx : INDEX1; // second closest control point
	uint far_idx : INDEX2; // furthest distance control point
};

#include "Field.hlsl"

// Main

float3 getCtrlPtsSum(){
	float3 sum = float3(0.0, 0.0, 0.0);
	for(uint n = 0; n < 16; n++) sum += ctrlPoints[n];
	return sum;
}

float getCtrlPtsLengths(){
	float lengthTotal = 0.0;
	for(uint n = 0; n < 16; n++) lengthTotal += length(ctrlPoints[n]);
	return lengthTotal;
}

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
	float4 outColor;

	float3 target = input.vertex_pos;
	if(mode < 0) target = input.texcoord;
 	
	float3 ctrlPt = ctrlPoints[input.near_idx];
	if(abs(mode) % 10 == 1) ctrlPt = ctrlPoints[input.far_idx];
	else if(abs(mode) % 10 == 2) ctrlPt = ctrlPoints[input.sec_idx];
	else if(abs(mode) % 10 == 3) ctrlPt = getCtrlPtsSum();
	else if(abs(mode) % 10 == 4) ctrlPt *= getCtrlPtsLengths();
	else if(abs(mode) % 10 == 5) ctrlPt += ctrlPoints[input.sec_idx] - ctrlPoints[input.far_idx];
	else if(abs(mode) % 10 == 6) ctrlPt *= ctrlPoints[input.sec_idx] * ctrlPoints[input.far_idx];
	else if(abs(mode) % 10 == 7) ctrlPt = smoothstep(ctrlPt, ctrlPoints[input.sec_idx], ctrlPoints[input.far_idx]);
	else if(abs(mode) % 10 == 8) ctrlPt = lerp(ctrlPt, ctrlPoints[input.sec_idx], length(ctrlPoints[input.far_idx]));
	else if(abs(mode) % 10 == 9) ctrlPt = cross(ctrlPt, ctrlPoints[input.sec_idx]);
	float3 relCoord = ctrlPt - target;

	uint m = abs(mode) / 10;
	if(m % 10 == 1) outColor = field1(ctrlPt, target * target);
	else if(m % 10 == 2) outColor = field2(ctrlPt, target * target);
	else if(m % 10 == 3) outColor = field3(ctrlPt, target * target);
	else if(m % 10 == 4) outColor = field4(ctrlPt, target * target);
	else if(m % 10 == 5) outColor = field5(ctrlPt, target * target);
	else if(m % 10 == 6) outColor = field6(ctrlPt, target * target);
	else if(m % 10 == 7) outColor = field7(ctrlPt, target * target);
	else if(m % 10 == 8) outColor = field8(ctrlPt, target * target);
	else if(m % 10 == 9) outColor = field9(ctrlPt, target * target);
	// else if(m >= 800 && m < 900) outColor = float4(pow(relCoord.x, relCoord.y), pow(relCoord.y, relCoord.z), pow(relCoord.z, relCoord.x), 1.0);
	else outColor = float4(length(relCoord), length(relCoord * 2), length(relCoord * 3), 1.0);

	outColor = float4(abs(outColor.r) - floor(outColor.r), abs(outColor.g) - floor(outColor.g), abs(outColor.b) - floor(outColor.b), outColor.a); // clamp
#ifdef INCLUDE_TEXTURES
	// outColor = smoothstep(outColor, modalTex(mode / 1000, outColor.rgb), float4(relCoord, 0.5)); // modulate with texture
	outColor *= modalTex(abs(mode / 1000), input.texcoord);
#endif
	return outColor;
}