#define INCLUDE_BLOCK
#define INCLUDE_TEXTURES
#define IGNORE_INPUTS

// #include <Beams>

#include "Common.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	float3 lightPos;
	float3 lightVal;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 vertex_pos : POSITION;
	float3 normal : NORMAL;
	float3 texcoord : TEXCOORD;
};

// Functions

float calcSpec(float4 camera, float3 vertex, float focus) { // Custom Function
	float intensity = dot(normalize(float3(camera.x, camera.y, camera.z)), normalize(vertex)) * focus;
	return max(pow(intensity, 3), 0);
}

float calcDiffuse(float3 light, float3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Sampler Functions

float4 sampleTex(uint sampleMode, float3 texcoords){
    if(abs(sampleMode) % 8 == 1) return color_correct(tex1.Sample(sampler1, float2(texcoords.x, texcoords.y)));
    else if(abs(sampleMode) % 8 == 2) return color_correct(tex2.Sample(sampler2, float2(texcoords.x, texcoords.y)));
    else if(abs(sampleMode) % 8 == 3) return color_correct(tex3.Sample(sampler3, float2(texcoords.x, texcoords.y)));
    else if(abs(sampleMode) % 8 == 4) return color_correct(tex4.Sample(sampler4, float2(texcoords.x, texcoords.y)));
    else if(abs(sampleMode) % 8 == 5) return color_correct(tex5.Sample(sampler5, float2(texcoords.x, texcoords.y)));
    else if(abs(sampleMode) % 8 == 6) return color_correct(tex6.Sample(sampler6, float2(texcoords.x, texcoords.y)));
    else if(abs(sampleMode) % 8 == 7) return color_correct(tex7.Sample(sampler7, float2(texcoords.x, texcoords.y)));
    else return color_correct(baseTex.Sample(baseSampler, float2(texcoords.x, texcoords.y)));
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
    uint4 modes = getModes(mode);
	uint intensity = modes[2] + 1;

	float3 target;
	if(mode >= 0) target = input.normal; else target = input.vertex_pos;

	Texture2D textures[8 - 1] = { tex1, tex2, tex3, tex4, tex5, tex6, tex7 }; // TODO: Change order?
	SamplerState samplers[8 - 1] = { sampler1, sampler2, sampler3, sampler4, sampler5, sampler6, sampler7 }; // TODO: Change order?

	float3 texVals[3];
	texVals[0] = sampleTex(abs(mode) + 1, input.texcoord);
	texVals[1] = sampleTex(abs(mode) + 2, input.texcoord);
	texVals[2] = sampleTex(abs(mode) + 3, input.texcoord);

	float3 color = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y)));
	float3 ambient = (lightVal * texVals[0]) * (0.25 + (0.05 * intensity));
	float3 diffuse = (lightVal * texVals[1]) * calcDiffuse(lightPos, target - offset) * 0.5 * intensity;
	float3 specular = (lightVal * texVals[2]) * calcSpec(cam_pos, target, float(intensity + 1) * 0.5);

	// TODO: Calculate light interaction with texture

	return float4((color + abs(ambient + diffuse + specular)) / 2.0, 1.0);
}