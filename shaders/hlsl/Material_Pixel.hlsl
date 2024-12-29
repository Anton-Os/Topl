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

// Main

float4 main(PS_INPUT input) : SV_TARGET{
    uint4 modes = getModes(mode);
	uint intensity = modes[2] + 1;

	float3 target;
	if(mode >= 0) target = input.normal; else target = input.vertex_pos;

	float3 texVals[3];
	texVals[0] = color_correct(tex1.Sample(sampler1, float2(input.texcoord.x, input.texcoord.y)));
	texVals[1] = color_correct(tex2.Sample(sampler2, float2(input.texcoord.x, input.texcoord.y)));
	texVals[2] = color_correct(tex3.Sample(sampler3, float2(input.texcoord.x, input.texcoord.y)));

	float3 color = color_correct(baseTex.Sample(baseSampler, float2(input.texcoord.x, input.texcoord.y)));
	float3 ambient = (lightVal * texVals[0]) * (0.25 + (0.05 * intensity));
	float3 diffuse = (lightVal * texVals[1]) * calcDiffuse(lightPos, target - offset) * 0.5 * intensity;
	float3 specular = (lightVal * texVals[2]) * calcSpec(cam_pos, target, float(intensity + 1) * 0.5);

	// TODO: Calculate light interaction with texture

	return float4((color + abs(ambient + diffuse + specular)) / 2.0, 1.0);
}