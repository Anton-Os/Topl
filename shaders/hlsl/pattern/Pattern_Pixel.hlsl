#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS

#define PATTERN_SIZE 0.025

#include "Common.hlsl"

#include "Pixel.hlsl"

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

    double timeFrame;
	double timeElapse;
	float2 cursorPos;
}

struct PS_INPUT { 
	float4 pos : SV_POSITION; 
	float3 vertex_pos: POSITION1;
	float3 vertex_color : COLOR;
	float3 normal: NORMAL;
	float3 texcoord: TEXCOORD;
	float3 tangent: TANGENT;
};

// Main

#include "pattern/Pattern.hlsl"

float3 patternChoice(float3 coords, int m, float t){
#include <Custom_Pattern>
	if(m % 10 == 1) return pattern1(coords, t);
	else if(m % 10 == 2) return pattern2(coords, t);
	else if(m % 10 == 3) return pattern3(coords, t);
	else if(m % 10 == 4) return pattern4(coords, t);
	else if(m % 10 == 5) return pattern5(coords, t);
	else if(m % 10 == 6) return pattern6(coords, t);
	else if(m % 10 == 7) return pattern7(coords, t);
	else if(m % 10 == 8) return pattern8(coords, t);
	else if(m % 10 == 9) return pattern9(coords, t);
	else return coords;
}

float4 main(PS_INPUT input, uint primID : SV_PrimitiveID) : SV_TARGET{
	float4 outColor;
	float3 coords = input.vertex_pos; // float3(input.pos.x, input.pos.y, input.pos.z);

	// return float4(input.texcoord * input.vertex_color, 1.0);
	if(abs(mode) % 10 == 1) coords = float3(input.pos.x, input.pos.y, input.pos.z);
	else if(abs(mode) % 10 == 2) coords = input.vertex_color;
	else if(abs(mode) % 10 == 3) coords = input.normal;
	else if(abs(mode) % 10 == 4) coords = input.tangent;
	else if(abs(mode) % 10 == 5) coords = input.texcoord;
	else if(abs(mode) % 10 == 6) coords = getRandColor(primID);
	else if(abs(mode) % 10 == 7) coords = (input.vertex_pos * input.texcoord) + (input.normal / input.tangent);
	else if(abs(mode) % 10 == 8) coords = float3(pow(input.pos.x, input.texcoord.x), pow(input.pos.y, input.normal.y), pow(input.pos.z, input.tangent.z));
	else if(abs(mode) % 10 == 9) coords = float3(sin(input.pos.x * input.vertex_pos.x), cos(input.pos.y * input.vertex_color.g), tan(input.pos.z * primID));

	float4 srcColor = float4(patternChoice(coords, abs(mode), timeElapse), 1.0);
	if(mode > 0) srcColor = float4(patternChoice(float3(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z), abs(mode), timeElapse), 1.0);
	// if(mode > 0) srcColor = float4(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z, 1.0); // test

	float4 dstColor = float4(patternChoice(coords, abs(mode / 10), timeElapse), 1.0); // srcColor;
	if(mode < 0) dstColor = float4(patternChoice(float3(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z), abs(mode), timeElapse), 1.0);
	// if(mode < 0) dstColor = float4(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z, 1.0); // test

	// Mixing Algorithm
	/* if(abs(mode / 100) % 10 == 1) outColor = srcColor + dstColor;
	else if(abs(mode / 100) % 10 == 2) outColor = srcColor - dstColor;
	else if(abs(mode / 100) % 10 == 3) outColor = srcColor * dstColor;
	else if(abs(mode / 100) % 10 == 4) outColor = srcColor / dstColor;
	else if(abs(mode / 100) % 10 == 5) outColor = float4(sin(srcColor.r * dstColor.r), cos(srcColor.g * dstColor.g), tan(srcColor.b * dstColor.b), 1.0);
	else if(abs(mode / 100) % 10 == 6) outColor = float4(pow(srcColor.r, dstColor.x), pow(srcColor.g, dstColor.y), pow(srcColor.b, dstColor.z), 1.0);
	else if(abs(mode / 100) % 10 == 7) outColor = float4(srcColor.r + dstColor.x, srcColor.g - dstColor.g, srcColor.b * dstColor.z, 1.0);
	else if(abs(mode / 100) % 10 == 8) outColor = float4(cross(float3(srcColor.r, srcColor.g, srcColor.b), float3(dstColor.r, dstColor.g, dstColor.b)), 1.0);
	else if(abs(mode / 100) % 10 == 9) outColor = float4(smoothstep(float3(srcColor.r, srcColor.g, srcColor.b), float3(dstColor.r, dstColor.g, dstColor.b), (mode % 100) / 100.0), 1.0);
	else outColor = dstColor; */

	outColor = float4(smoothstep(srcColor, dstColor, cross(srcColor, dstColor)), length(offset - coords));
	// outColor = float4(smoothstep(srcColor, dstColor, offset - coords), 1.0);

	return outColor;
}