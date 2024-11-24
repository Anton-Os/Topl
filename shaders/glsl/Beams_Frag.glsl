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
layout(location = 1) in vec3 vertex_pos;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 color;

// Functions

float calcSpec(vec3 light, vec3 vertex, float intensity) {
	vec3 reflectVec = light - (normalize(vertex) * 2 * dot(light, normalize(vertex)));
	if(mode < 0) return max(pow(dot(reflectVec, -normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z))), 1.0), 0);
	else return max(pow(dot(reflectVec, -normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z))), intensity), 0);
}

float calcDiffuse(vec3 light, vec3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

void main() {
	uvec4 modes = getModes(mode);
	uint intensity = modes[2] + 1;

	vec3 target;
	if(mode >= 0) target = normal; else target = vertex_pos; // set target conditionally

	vec3 lights[3][2]; // populated and sorted list of lights
    if(modes[1] % 3 == 1){ lights[0] = flashLight; lights[1] = lampLight; lights[2] = skyLight; }
    else if(modes[1] % 3 == 2){ lights[0] = lampLight; lights[1] = skyLight; lights[2] = flashLight; }
    else{ lights[0] = skyLight; lights[1] = flashLight; lights[2] = lampLight; }

	vec3 ambient = lights[0][1] * (0.25 + (0.05 * intensity));
	vec3 diffuse = lights[0][1] * calcDiffuse(lights[0][0], target - offset) * 0.5 * intensity;
	vec3 specular = lights[0][1] * calcSpec(lights[0][0], target, float(1 + intensity));

	if(modes[1] >= 3){ // combining lights
        uint count = 2;
        if(modes[1] > 6) count = 3;
        for(uint l = 1; l < count; l++){
			float attenuation = 1.0 / count;
			// determining total light
            ambient += (lights[l % 3][1] * (0.25 + (0.05 * intensity))) * attenuation;
            diffuse += (lights[l % 3][1] * calcDiffuse(lights[l][0], target - offset) * 0.5 * intensity) * attenuation;
            specular += (lights[l % 3][1] * calcSpec(vec3(cam_pos.x, cam_pos.y, cam_pos.z), target, float(1 + intensity))) * attenuation;
        }
    }

	if(modes[0]== 1) color = vec4(ambient, 1.0f); // ambient mode
	else if(modes[0] == 2) color = vec4(diffuse, 1.0f); // diffuse mode
	else if(modes[0] == 3) color = vec4(specular, 1.0f); // specular mode
	else if(modes[0] == 4) color = vec4(lights[0][1] * dot(normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z)), normalize(target)), 1.0); // highlight mode
	else if(modes[0] == 5) color = vec4(ambient.r + pow(specular.r, 1.0 / diffuse.r), ambient.g + pow(specular.g, 1.0 / diffuse.g), ambient.b + pow(specular.b, 1.0 / diffuse.b), 1.0); // spot mode
	else if(modes[0] == 6){ // depth mode
		float depth = sqrt(pow(target.x, 2) + pow(target.y, 2) + pow(target.z, 2)); // depth calculation
		color = vec4(depth, depth, depth, 1.0f);
	}
	else if(modes[0] == 7){ // distance mode
		vec3 distVec = lights[0][0] - target - offset;
		float dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));
		color = vec4(ambient + (lights[0][1] * (1.0 - dist)), 1.0);
		// vec3 outputColor = lights[0][1] * (1.0 - (dist * (1.0 / pow(abs(mode), 0.5)))), 1.0 - (dist * (1.0 / pow(abs(mode), 0.5)));
	}
	else if(modes[0] == 8) color = vec4(normalize(cross(vec3(cam_pos.x, cam_pos.y, cam_pos.z) - lights[0][0], target)), 1.0); // relative mode
	else if(modes[0] == 9) color = vec4(ambient + vec3(sin(specular.r / diffuse.r), cos(diffuse.g / specular.g), tan(diffuse.b + specular.b)), 1.0); // experimental mode
	else color = vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
