#define INCLUDE_EXTBLOCK
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

#include "field/Field.hlsl"

// Main

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
	float4 outColor;

	uint m = abs(mode);
	float3 target = input.vertex_pos;
	if(mode < 0) target = input.texcoord;
 	
	float3 nearPoint = ctrlPoints[input.near_idx];
	float3 relCoord = nearPoint - target;

	if(m % 10 == 1) outColor = field1(nearPoint, target);
	else if(m % 10 == 2) outColor = field2(nearPoint, target);
	else if(m % 10 == 3) outColor = field3(nearPoint, target);
	else if(m % 10 == 4) outColor = field4(nearPoint, target);
	else if(m % 10 == 5) outColor = field5(nearPoint, target);
	else if(m % 10 == 6) outColor = field6(input.near_idx, input.sec_idx, input.far_idx, target);
	else if(m % 10 == 7) outColor = field7(input.near_idx, input.sec_idx, input.far_idx, target);
	else if(m % 10 == 8) outColor = field8(input.near_idx, input.sec_idx, input.far_idx, target);
	else if(m % 10 == 9) outColor = field9(input.near_idx, input.sec_idx, input.far_idx, target);
	// else if(m >= 800 && m < 900) outColor = float4(pow(relCoord.x, relCoord.y), pow(relCoord.y, relCoord.z), pow(relCoord.z, relCoord.x), 1.0);
	else outColor = float4(abs(relCoord.x) - floor(abs(relCoord.x)), abs(relCoord.y) - floor(abs(relCoord.y)), abs(relCoord.z) - floor(abs(relCoord.z)), 1.0);

	outColor = float4(abs(outColor.r) - floor(outColor.r), abs(outColor.g) - floor(outColor.g), abs(outColor.b) - floor(outColor.b), outColor.a); // clamp
#ifdef INCLUDE_TEXTURES
	// outColor = smoothstep(outColor, modalTex(mode / 1000, outColor.rgb), float4(relCoord, 0.5)); // modulate with texture
	outColor *= modalTex(abs(mode / 100), input.texcoord);
#endif
	return outColor;
}