#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock{
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	vec3 skyLight[2]; // position and value
	vec3 flashLight[2]; // position and value
	vec3 lampLight[2]; // position and value
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 light_pos;
layout(location = 2) in vec3 light_val;
layout(location = 3) in float light_dist;

layout(location = 0) out vec4 color;

// Functions

float calcSpec(vec3 light, vec3 vertex) {
	vec3 reflectVec = light - (normalize(vertex) * 2 * dot(light, normalize(vertex)));
	return max(pow(dot(reflectVec, -normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z))), 3), 0) * 1.0;
}

float calcDiffuse(vec3 light, vec3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

void main() {
	vec3 ambient = light_val * 0.2;
	vec3 diffuse = light_val * calcDiffuse(light_pos, pos) * 0.5;
	vec3 specular = light_val * calcSpec(light_pos, pos);

	if(mode == 1) color = vec4(ambient, 1.0f);
	else if(mode == 2) color = vec4(diffuse, 1.0f);
	else if(mode == 3) color = vec4(specular, 1.0f);
	else if(mode == 4){ // depth mode
		float depth = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2)); // depth calculation
		color = vec4(depth, depth, depth, 1.0f);
	}
	else if(mode == 5) color = vec4(light_val, 1.0); // light value
	// else if(mode == 6) color = // TODO: Implement this
	// else if(mode == 7) color = // TODO: Implement this
	// else if(mode == 8) color = // TODO: Implement this
	else if(mode < 0 && mode > -8){
		vec3 illumin = light_val * abs(mode) * (1.0 - light_dist) * 0.1;
		color = vec4(illumin, 1.0f);
	}
	else color = vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
