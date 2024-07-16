#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	float3 skyLight_pos; float3 skyLight_value;
	float3 flashLight_pos; float3 flashLight_value;
	float3 lampLight_pos; float3 lampLight_value;
}

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 vertex_pos : POSITION;
	float3 light_pos : LIGHT1; // light position
	float3 light_val: LIGHT2; // light value
};

// Functions

float calcSpec(float4 camera, float3 vertex) { // Custom Function
	float intensity = dot(normalize(float3(camera.x, camera.y, camera.z)), normalize(vertex));
	return max(pow(intensity * 5, 5), 0);
	// return max(pow(intensity, 3), 0);
}

float calcDiffuse(float3 light, float3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float3 ambient = input.light_val * 0.2;
	float3 diffuse = input.light_val * calcDiffuse(input.light_pos, input.vertex_pos - offset) * 0.5;
	float3 specular = input.light_val * calcSpec(cam_pos, input.vertex_pos);

	if(mode == 1) return float4(ambient, 1.0f);
	else if(mode == 2) return float4(diffuse, 1.0f);
	else if(mode == 3) return float4(specular, 1.0f);
	else if(mode == 4){ // depth mode
		float depth = sqrt(pow(input.vertex_pos.x, 2) + pow(input.vertex_pos.y, 2) + pow(input.vertex_pos.z, 2)); // depth calculation
		return float4(depth, depth, depth, 1.0f);
	}
	else if(mode == 5) return float4(input.light_val, 1.0); // light value
	// else if(mode == 6) return input.light_val / distance; // light distance value
	else return float4(ambient + diffuse + specular, 1.0); // all lighting
}