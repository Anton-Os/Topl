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

float4 pattern_effect(float3 coords, int m, float t){
#include <Custom_Pattern>
	if(coords.x == 0 && coords.y == 0 && coords.z == 0) return float4(0, 0, 0, 0.0); // returns transparent if coords are zeroed out by Custom_Pattern
	else if(m % 10 == 1) return float4(pattern1(coords, t), 1.0);
	else if(m % 10 == 2) return float4(pattern2(coords, t), 1.0);
	else if(m % 10 == 3) return float4(pattern3(coords, t), 1.0);
	else if(m % 10 == 4) return float4(pattern4(coords, t), 1.0);
	else if(m % 10 == 5) return float4(pattern5(coords, t), 1.0);
	else if(m % 10 == 6) return float4(pattern6(coords, t), 1.0);
	else if(m % 10 == 7) return float4(pattern7(coords, t), 1.0);
	else if(m % 10 == 8) return float4(pattern8(coords, t), 1.0);
	else if(m % 10 == 9) return float4(pattern9(coords, t), 1.0);
	else return float4(coords, 1.0);
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
	else if(abs(mode) % 10 == 8) coords = float3(pow(abs(input.vertex_pos.x), abs(input.tangent.x)), pow(abs(input.texcoord.y), abs(input.tangent.y)), pow(abs(input.normal.z), abs(input.tangent.z)));
	else if(abs(mode) % 10 == 9) coords = float3(sin(input.pos.x * input.vertex_pos.x), cos(input.pos.y * input.vertex_color.g), tan(input.pos.z * primID));

	float4 srcColor = pattern_effect(coords, abs(mode), timeElapse);
	if(mode > 0) srcColor = pattern_effect(float3(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z), abs(mode), timeElapse);
	// if(mode > 0) srcColor = float4(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z, 1.0); // test

	float4 dstColor = pattern_effect(coords, abs(mode / 10), timeElapse); // srcColor;
	if(mode < 0) dstColor = pattern_effect(float3(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z), abs(mode), timeElapse);
	// if(mode < 0) dstColor = float4(cursorPos.x - coords.x, cursorPos.y - coords.y, sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2)) - coords.z, 1.0); // test

	// Mixing Algorithm
	if(abs(mode / 1000) % 10 == 1) outColor = srcColor + dstColor;
	else if(abs(mode / 1000) % 10 == 2) outColor = float4(smoothstep(srcColor, dstColor, cross(srcColor, dstColor)), length((offset + coords) * 0.5)); // srcColor - dstColor;
	else if(abs(mode / 1000) % 10 == 3) outColor = float4(smoothstep(srcColor, dstColor, cross(srcColor, dstColor)), length(offset * coords)); // srcColor * dstColor;
	else if(abs(mode / 1000) % 10 == 4) outColor = float4(smoothstep(srcColor, dstColor, cross(srcColor, dstColor)), length(offset / coords)); // srcColor / dstColor;
	else if(abs(mode / 1000) % 10 == 5) outColor = float4(sin(srcColor.r * dstColor.r), cos(srcColor.g * dstColor.g), tan(srcColor.b * dstColor.b), 1.0);
	else if(abs(mode / 1000) % 10 == 6) outColor = float4(pow(srcColor.r, dstColor.x), pow(srcColor.g, dstColor.y), pow(srcColor.b, dstColor.z), 1.0);
	else if(abs(mode / 1000) % 10 == 7) outColor = float4(srcColor.r + dstColor.x, srcColor.g - dstColor.g, srcColor.b * dstColor.z, 1.0);
	else if(abs(mode / 1000) % 10 == 8) outColor = float4(cross(float3(srcColor.r, srcColor.g, srcColor.b), float3(dstColor.r, dstColor.g, dstColor.b)), 1.0);
	else if(abs(mode / 1000) % 10 == 9) outColor = float4(smoothstep(float3(srcColor.r, srcColor.g, srcColor.b), float3(dstColor.r, dstColor.g, dstColor.b), (mode % 100) / 100.0), 1.0);
	else outColor = float4(smoothstep(srcColor, dstColor, cross(srcColor, dstColor)), length(offset - coords));

	return outColor;
}