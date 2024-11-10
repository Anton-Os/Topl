#define INCLUDE_TEXTURES
#define INCLUDE_SCENEBLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;

	float3 texScroll; // texture coordinate scrolling
	float3 texScale; // texture coordinate scaling
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 color_blend(float4 color1, float4 color2){ return (color1 + color2) / 2; }

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(mode % 10 == 8) return color_correct(areaTex.Sample(areaSampler, input.texcoord)); // volumetric texture
	else if(mode % 10 == 9) return color_correct(areaTex.Sample(areaSampler, float3(input.texcoord.x, input.texcoord.y, 0.0))); // volumetric slice
	else {
		if(mode % 10 == 1) return color_correct(tex1.Sample(sampler1, float2(input.texcoord.x, input.texcoord.y)));
		else if(mode % 10 == 2) return color_correct(tex2.Sample(sampler2, float2(input.texcoord.x, input.texcoord.y)));
		else if(mode % 10 == 3) return color_correct(tex3.Sample(sampler3, float2(input.texcoord.x, input.texcoord.y)));
		else if(mode % 10 == 4) return color_correct(tex4.Sample(sampler4, float2(input.texcoord.x, input.texcoord.y)));
		else if(mode % 10 == 5) return color_correct(tex5.Sample(sampler5, float2(input.texcoord.x, input.texcoord.y)));
		else if(mode % 10 == 6) return color_correct(tex6.Sample(sampler6, float2(input.texcoord.x, input.texcoord.y)));
		else if(mode % 10 == 7) return color_correct(tex7.Sample(sampler7, float2(input.texcoord.x, input.texcoord.y)));
		else return color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // base texture
	}
}