#version 440

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

#include "Common.glsl"

#include "Pixel.glsl"

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
	vec3 diffuse = lights[0][1] * getDiffuse(lights[0][0], target - offset) * 0.5 * intensity;
	vec3 specular = lights[0][1] * getSpecular(lights[0][0], vec3(cam_pos), target, float(1 + intensity));

	if(modes[1] >= 3){ // combining lights
        uint count = 2;
        if(modes[1] > 6) count = 3;
        for(uint l = 1; l < count; l++){
			float attenuation = 1.0 / count;
			// determining total light
            ambient += (lights[l % 3][1] * (0.25 + (0.05 * intensity))) * attenuation;
            diffuse += (lights[l % 3][1] * getDiffuse(lights[l][0], target - offset) * 0.5 * intensity) * attenuation;
            specular += (lights[l % 3][1] * getSpecular(lights[l][0], vec3(cam_pos), target, float(1 + intensity))) * attenuation;
        }
    }

	if(modes[0]== BEAMS_AMBIENT) color = vec4(ambient, 1.0f);
	else if(modes[0] == BEAMS_DIFFUSE) color = vec4(diffuse, 1.0f);
	else if(modes[0] == BEAMS_SPECULAR) color = vec4(specular, 1.0f);
	else if(modes[0] == BEAMS_HIGHLIGHT) color = vec4(lights[0][1] * dot(normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z)), normalize(target)), 1.0);
	else if(modes[0] == BEAMS_SPOT) color = vec4(ambient.r + pow(specular.r, 1.0 / diffuse.r), ambient.g + pow(specular.g, 1.0 / diffuse.g), ambient.b + pow(specular.b, 1.0 / diffuse.b), 1.0);
	else if(modes[0] == BEAMS_DEPTH) color = vec4(distance(target, vec3(cam_pos)), distance(target, vec3(cam_pos)), distance(target, vec3(cam_pos)), 1.0f);
	else if(modes[0] == BEAMS_DISTANCE) color =  vec4(ambient + (distance(target, vec3(cam_pos)) * diffuse) + specular, 1.0f);
	else if(modes[0] == BEAMS_TRAJECTORY) color = vec4(normalize(cross(vec3(cam_pos.x, cam_pos.y, cam_pos.z) - lights[0][0], target)), 1.0);
	else if(modes[0] == BEAMS_TRIAL) color = vec4(ambient + vec3(sin(specular.r / diffuse.r), cos(diffuse.g / specular.g), tan(diffuse.b + specular.b)), 1.0); 
	else color = vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
