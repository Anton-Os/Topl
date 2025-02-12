#define INCLUDE_BLOCK
#define INCLUDE_TEXTURES
#define IGNORE_INPUTS

// #include <Beams>

#include "../Common.hlsl"

#include "Pixel.hlsl"

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
	uint t = modes[1];
	uint intensity = modes[2] + 1;

	float3 target;
	if(mode >= 0) target = input.normal; else target = input.vertex_pos;

	Texture2D textures[8 - 1] = { tex1, tex2, tex3, tex4, tex5, tex6, tex7 }; // TODO: Change order?
	SamplerState samplers[8 - 1] = { sampler1, sampler2, sampler3, sampler4, sampler5, sampler6, sampler7 }; // TODO: Change order?

	float3 texVals[8];
	for(int o = 0; o < 8; o++){
		float4 texTarget = modalTex((abs(mode) + o) % 8, input.texcoord);
		texVals[o] =  float3(texTarget.r, texTarget.g, texTarget.b);
	}

	float3 color = texVals[0];
	float3 ambientColor = lightVal * texVals[(1 + t) % 8];
	float3 ambient = ambientColor * (0.25 + (0.05 * intensity));
	float3 diffuseColor = lightVal * texVals[(2 + t) % 8];
	float3 diffuse = diffuseColor * calcDiffuse(lightPos - color_range(texVals[(3 + t) % 8]), (target - offset) - color_range(texVals[(4 + t) % 8])) * 0.5 * intensity;
	float3 specularColor = lightVal * texVals[(5 + t) % 8];
	float3 specular = specularColor * calcSpec(cam_pos - float4(color_range(texVals[(6 + t) % 8]), 1.0), target - color_range(texVals[(7 + t) % 8]), float(intensity + 1) * 0.5);

	return float4((color + abs(ambient + diffuse + specular)) / 2.0, 1.0);
}