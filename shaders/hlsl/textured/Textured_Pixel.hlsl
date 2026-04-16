#define INCLUDE_SCENEBLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_BLOCK : register(b0) {
	float3 offset;
	float3 rotation;
	float3 scale;

	float3 texScroll; // texture coordinate scrolling
	float4 texScale; // texture coordinate scaling

    uint flip;
    float antialiasArea;
    uint antialiasSteps;
	float slice;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 color_blend(float4 color1, float4 color2){ return (color1 + color2) / 2; }

float4 color_checker(float3 texcoord, uint tileCount){
	uint c = (tileCount + 1) * 2;
	if(fmod(floor(texcoord.x * c), 2.0) != fmod(floor(texcoord.y * c), 2.0)) // return float4(1, 1, 1, 1);
		return float4(getRandColor(uint(floor(texcoord.x * c))).r, getRandColor(uint(floor(texcoord.y * c))).g, getRandColor(uint(floor(texcoord.z * c))).b, 1.0);
	else return float4(0, 0, 0, 0.5); 
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
#ifdef INCLUDE_TEXTURES
	if(abs(mode) % 10 == 8) return antialias3D(input.texcoord, areaTex, areaSampler, antialiasArea, antialiasSteps);
	else if(abs(mode) % 10 == 9) return antialias3D(float3(input.texcoord.x, input.texcoord.y, slice), areaTex, areaSampler, antialiasArea, antialiasSteps);
	else {
		if(abs(mode) % 10 == 1) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex1, sampler1, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 2) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex2, sampler2, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 3) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex3, sampler3, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 4) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex4, sampler4, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 5) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex5, sampler5, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 6) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex6, sampler6, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 7) return antialias2D(float2(input.texcoord.x, input.texcoord.y), tex7, sampler7, antialiasArea, antialiasSteps);
		else return antialias2D(float2(input.texcoord.x, input.texcoord.y), baseTex, baseSampler, antialiasArea, antialiasSteps);
	}
#else
	return color_checker(input.texcoord, uint(abs(mode)));
#endif
}