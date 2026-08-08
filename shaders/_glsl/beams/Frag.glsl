#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

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
layout(location = 3) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

uint getLightCount(uint mode){ // used for determining how many lights to combine
    uint count = 1;
    if(mode >= 3) count = 2;
    if(mode >= 6) count = 3;
    return count;
}

#include "beams/Beams.glsl"

// Main

void main() {
	uvec4 modes = getModes(mode);
	uint intensity = modes[3] + 1;

	vec3 target;
	if(mode >= 0) target = normal; 
	else target = vertex_pos; 

	vec3 lights[3][2]; // populated and sorted list of lights
    if(modes[1] % 3 == 1){ lights[0] = flashLight; lights[1] = lampLight; lights[2] = skyLight; }
    else if(modes[1] % 3 == 2){ lights[0] = lampLight; lights[1] = skyLight; lights[2] = flashLight; }
    else{ lights[0] = skyLight; lights[1] = flashLight; lights[2] = lampLight; }

#ifdef INCLUDE_TEXTURES
	vec3 ambient = getAmbient_sampled(lights, texcoord, intensity);
	vec3 diffuse = getDiffuse_sampled(lights, target, intensity);
	vec3 specular = getSpecular_sampled(lights, target, intensity);
#else
	vec3 ambient = getAmbient_flat(lights, intensity);
	vec3 diffuse = getDiffuse_flat(lights, target, intensity);
	vec3 specular = getSpecular_flat(lights, target, intensity);
#endif

	if(modes[0]== 1) color_final = vec4(ambient, 1.0f);
	else if(modes[0] == 2) color_final = vec4(diffuse, 1.0f);
	else if(modes[0] == 3) color_final = vec4(specular, 1.0f);
	else if(modes[0] == 4) color_final = vec4(lights[0][1] * dot(normalize(vec3(cam_pos.x, cam_pos.y, cam_pos.z)), normalize(target)), 1.0);
	else if(modes[0] == 5) color_final = vec4(ambient.r + pow(specular.r, 1.0 / diffuse.r), ambient.g + pow(specular.g, 1.0 / diffuse.g), ambient.b + pow(specular.b, 1.0 / diffuse.b), 1.0);
	else if(modes[0] == 6) color_final = vec4(vec3(distance(target, vec3(cam_pos)), distance(target, vec3(cam_pos)), distance(target, vec3(cam_pos))) * lights[0][1], 1.0f);
	else if(modes[0] == 7) color_final =  vec4(ambient + (distance(target, vec3(cam_pos)) * diffuse) + specular, 1.0f);
	else if(modes[0] == 8) color_final = vec4(ambient + (distance(target, lights[0][0]) * diffuse) + (specular * cross(target, lights[0][0])), 1.0f);
	// else if(modes[0] == 8) color_final = vec4(lights[0][1] * normalize(cross(vec3(cam_pos.x, cam_pos.y, cam_pos.z) - lights[0][0], target)), 1.0);
	else if(modes[0] == 9) color_final = vec4(ambient + vec3(sin(specular.r / diffuse.r), cos(diffuse.g / specular.g), tan(diffuse.b + specular.b)), 1.0); 
	else color_final = vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
