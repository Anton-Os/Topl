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
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 color;

// Functions

float calcSpec(vec3 light, vec3 vertex, float intensity) {
	vec3 reflectVec = light - (normalize(vertex) * 2 * dot(light, normalize(vertex)));
	return max(pow(dot(reflectVec, -normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z))), intensity), 0);
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

	vec3 target;
	if(mode >= 0) target = normal; else target = pos; // set target conditionally

	vec3 lights[3][2]; // populated and sorted list of lights
    if(modes[1] % 3 == 1){ lights[0] = flashLight; lights[1] = lampLight; lights[2] = skyLight; }
    else if(modes[1] % 3 == 2){ lights[0] = lampLight; lights[1] = skyLight; lights[2] = flashLight; }
    else{ lights[0] = skyLight; lights[1] = flashLight; lights[2] = lampLight; }

	vec3 ambient = lights[0][1] * 0.3;
	vec3 diffuse = lights[0][1] * calcDiffuse(lights[0][0], target) * 0.5;
	vec3 specular = lights[0][1] * calcSpec(lights[0][0], target, 2.0);

	if(modes[1] >= 3){ // combining lights
        uint count = 2;
        if(modes[1] > 6) count = 3; // * (modes[1] - 6);
        for(uint l = 1; l < count; l++){
            ambient += (lights[l % 3][1] * 0.3) * (1.0 / count);
            diffuse += (lights[l % 3][1] * calcDiffuse(lights[l][0], pos - offset) * 0.5) * (1.0 / count);
            specular += (lights[l % 3][1] * calcSpec(vec3(cam_pos.x, cam_pos.y, cam_pos.z), target, 1.0)) * (1.0 / count);
        }
    }

	if(modes[0]== 1) color = vec4(ambient, 1.0f); // ambient mode
	else if(modes[0] == 2) color = vec4(diffuse, 1.0f); // diffuse mode
	else if(modes[0] == 3) color = vec4(specular, 1.0f); // specular mode
	else if(modes[0] == 4){ // depth mode
		float depth = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2)); // depth calculation
		color = vec4(depth, depth, depth, 1.0f);
	}
	else if(modes[0] == 5) color = vec4(lights[0][1], 1.0); // reference mode
	else if(modes[0] == 6) color = vec4(lights[0][1] * tan(dot(normalize(lights[0][0]), normalize(target))), 1.0); // trig mode
	else if(modes[0] == 7) color = vec4(lights[0][1] * (specular / diffuse) + ambient, 1.0);
	else if(modes[0] == 8) color = vec4(lights[0][1] * dot(normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z)), normalize(target)), 1.0);
	else if(modes[0] == 9){
		vec3 distVec = lights[0][0] - target - offset;
		float dist = sqrt(pow(distVec.x, 2) + pow(distVec.y, 2) + pow(distVec.z, 2));
		color = vec4(lights[0][1] * (1.0 - dist), 1.0);
		// vec3 outputColor = lights[0][1] * (1.0 - (dist * (1.0 / pow(abs(mode), 0.5)))), 1.0 - (dist * (1.0 / pow(abs(mode), 0.5)));
	}
	else color = vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
