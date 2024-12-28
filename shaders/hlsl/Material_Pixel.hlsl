#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#define BEAMS_FULL 0
#define BEAMS_AMBIENT 1
#define BEAMS_DIFFUSE 2
#define BEAMS_SPECULAR 3
#define BEAMS_HIGHLIGHT 4
#define BEAMS_SPOT 5
#define BEAMS_DEPTH 6
#define BEAMS_DISTANCE 7
#define BEAMS_TRAJECTORY 8
#define BEAMS_TRIAL 9

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

	float3 ambient = lightVal * (0.25 + (0.05 * intensity));
	float3 diffuse = lightVal * calcDiffuse(lightPos, target - offset) * 0.5 * intensity;
	float3 specular = lightVal * calcSpec(cam_pos, target, float(intensity + 1) * 0.5);

	return float4(ambient + diffuse + specular, 1.0);
}