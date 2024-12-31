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
	float4 texScale; // texture coordinate scaling

    float slice;
    uint flip;
    float antialiasArea;
    uint antialiasSteps;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float4 color_blend(float4 color1, float4 color2){ return (color1 + color2) / 2; }

float4 sampleTex2D(float2 coords, Texture2D tex, SamplerState samp){
	if(antialiasArea == 0.0 || antialiasSteps == 0) return color_correct(tex.Sample(samp, coords));
	else { // antialiasing algorithm
		float4 texColor = color_correct(tex.Sample(samp, coords));
		for(uint a = 0; a < antialiasSteps; a++){
			float f = (antialiasArea / antialiasSteps) * (a + 1);
			float4 nebrTexColors[8] = {
				color_correct(tex.Sample(samp, coords + float2(f, 0.0))), color_correct(tex.Sample(samp, coords + float2(-f, 0.0))), // left and right
				color_correct(tex.Sample(samp, coords + float2(0.0, f))), color_correct(tex.Sample(samp, coords + float2(0.0, -f))), // top and bottom
				color_correct(tex.Sample(samp, coords + float2(f, f))), color_correct(tex.Sample(samp, coords + float2(-f, -f))), // top right and bottom left
				color_correct(tex.Sample(samp, coords + float2(-f, f))), color_correct(tex.Sample(samp, coords + float2(f, -f))) // top left and bottom right
			};
			for(uint n = 0; n < 8; n++) texColor += nebrTexColors[n]; // total
			texColor *= 1.0 / 8; // average
		}
		return texColor;
	}
}

float4 sampleTex3D(float3 coords, Texture3D tex, SamplerState samp){
	if(antialiasArea == 0.0 || antialiasSteps == 0) return color_correct(tex.Sample(samp, coords));
	else {
		float4 texColor = color_correct(tex.Sample(samp, coords));
		for(uint a = 0; a < antialiasSteps; a++){
			/* float f = (antialiasArea / antialiasSteps) * (a + 1);
			float4 nebrTexColors[(8 * 3) + 2] = {
				// slice neighbors
				color_correct(tex.Sample(samp, coords + float3(f, 0.0, 0.0))), color_correct(tex.Sample(samp, coords + float3(-f, 0.0, 0.0))), // left and right
				color_correct(tex.Sample(samp, coords + float3(0.0, f, 0.0))), color_correct(tex.Sample(samp, coords + float3(0.0, -f, 0.0))), // top and bottom
				color_correct(tex.Sample(samp, coords + float3(f, f, 0.0))), color_correct(tex.Sample(samp, coords + float3(-f, -f, 0.0))), // top right and bottom left
				color_correct(tex.Sample(samp, coords + float3(-f, f, 0.0))), color_correct(tex.Sample(samp, coords + float3(f, -f, 0.0))) // top left and bottom right
				// back neighbors
				color_correct(tex.Sample(samp, coords + float3(0.0f, 0.0, -f)))
				color_correct(tex.Sample(samp, coords + float3(f, 0.0, -f))), color_correct(tex.Sample(samp, coords + float3(-f, 0.0, -f))), // left and right
				color_correct(tex.Sample(samp, coords + float3(0.0, f, -f))), color_correct(tex.Sample(samp, coords + float3(0.0, -f, -f))), // top and bottom
				color_correct(tex.Sample(samp, coords + float3(f, f, -f))), color_correct(tex.Sample(samp, coords + float3(-f, -f, -f))), // top right and bottom left
				color_correct(tex.Sample(samp, coords + float3(-f, f, -f))), color_correct(tex.Sample(samp, coords + float3(f, -f, -f))) // top left and bottom right
				// back neighbors
				color_correct(tex.Sample(samp, coords + float3(0.0f, 0.0, f)))
				color_correct(tex.Sample(samp, coords + float3(f, 0.0, f))), color_correct(tex.Sample(samp, coords + float3(-f, 0.0, f))), // left and right
				color_correct(tex.Sample(samp, coords + float3(0.0, f, f))), color_correct(tex.Sample(samp, coords + float3(0.0, -f, f))), // top and bottom
				color_correct(tex.Sample(samp, coords + float3(f, f, f))), color_correct(tex.Sample(samp, coords + float3(-f, -f, f))), // top right and bottom left
				color_correct(tex.Sample(samp, coords + float3(-f, f, f))), color_correct(tex.Sample(samp, coords + float3(f, -f, f))) // top left and bottom right
			}
			for(uint n = 0; n < (8 * 3) + 2; n++) texColor += nebrTexColors[n]; // total
			texColor *= 1.0 / ((8 * 3) + 2); // average */
		}
		return texColor;
	}
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	if(abs(mode) % 10 == 8) return sampleTex3D(input.texcoord, areaTex, areaSampler);
	else if(abs(mode) % 10 == 9) return sampleTex3D(float3(input.texcoord.x, input.texcoord.y, 0.0), areaTex, areaSampler);
	else {
		if(abs(mode) % 10 == 1) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex1, sampler1);
		else if(abs(mode) % 10 == 2) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex2, sampler2);
		else if(abs(mode) % 10 == 3) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex3, sampler3);
		else if(abs(mode) % 10 == 4) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex4, sampler4);
		else if(abs(mode) % 10 == 5) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex5, sampler5);
		else if(abs(mode) % 10 == 6) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex6, sampler6);
		else if(abs(mode) % 10 == 7) return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), tex7, sampler7);
		else return sampleTex2D(float2(input.texcoord.x, input.texcoord.y), baseTex, baseSampler);
	}
}