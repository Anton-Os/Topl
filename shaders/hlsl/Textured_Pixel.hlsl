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
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 color_blend(float4 color1, float4 color2){ return (color1 + color2) / 2; }

float4 color_correct(float4 color){ // switch red and blue color values
	float t = color[0];
	color[0] = color[2]; color[2] = t;
	return color;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(mode == 8) return color_correct(areaTex.Sample(areaSampler, input.texcoord)); // volumetric texture
	else if(mode > 0 && mode < 8){
		if(mode == 1) return color_correct(tex1.Sample(sampler1, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 2) return color_correct(tex2.Sample(sampler2, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 3) return color_correct(tex3.Sample(sampler3, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 4) return color_correct(tex4.Sample(sampler4, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 5) return color_correct(tex5.Sample(sampler5, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 6) return color_correct(tex6.Sample(sampler6, float2(input.texcoord.x, input.texcoord.y)));
		if(mode == 7) return color_correct(tex7.Sample(sampler7, float2(input.texcoord.x, input.texcoord.y)));
		else return float4(1.0, 0.0, 0.0, 1.0); // error
	}
	else if(mode < 0 && mode > -8){ // Alpha Values are layered
		float4 outColor = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y)));

		if(mode <= -1 && tex1.Sample(sampler1, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex1.Sample(sampler1, float2(input.texcoord.x, input.texcoord.y)), outColor));
		if(mode <= -2 && tex2.Sample(sampler2, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex2.Sample(sampler2, float2(input.texcoord.x, input.texcoord.y)), outColor));
		if(mode <= -3 && tex3.Sample(sampler3, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex3.Sample(sampler3, float2(input.texcoord.x, input.texcoord.y)), outColor));
		if(mode <= -4 && tex4.Sample(sampler4, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex4.Sample(sampler4, float2(input.texcoord.x, input.texcoord.y)), outColor));
		if(mode <= -5 && tex5.Sample(sampler5, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex5.Sample(sampler5, float2(input.texcoord.x, input.texcoord.y)), outColor));
		if(mode <= -6 && tex6.Sample(sampler6, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex6.Sample(sampler6, float2(input.texcoord.x, input.texcoord.y)), outColor));
		if(mode <= -7 && tex7.Sample(sampler7, float2(input.texcoord.x, input.texcoord.y)).a > 0.05)
			outColor = color_correct(color_blend(tex7.Sample(sampler7, float2(input.texcoord.x, input.texcoord.y)), outColor));
	
		return outColor;
	}
	else if(mode == 9) return float4(0.0, 0.0, 1.0, 0.5); // test
	else return color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y))); // base texture
}