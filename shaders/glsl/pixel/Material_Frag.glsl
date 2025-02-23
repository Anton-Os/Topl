#version 440

#define INCLUDE_BLOCK
#define INCLUDE_TEXTURES
#define IGNORE_INPUTS

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock{
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	vec3 texScroll; // texture coordinate scrolling
	vec4 texScale; // texture coordinate scaling

	vec3 lightPos;
	vec3 lightVal;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_pos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 texcoord;

layout(location = 0) out vec4 color;

// Main

void main() {
	uvec4 modes = getModes(mode);
	uint t = modes[1];
	uint intensity = modes[2] + 1;

	vec3 target;
	if(mode >= 0) target = normal; else target = vertex_pos; // set target conditionally

	vec3 texVals[8];
	for(int t = 0; t < 8; t++) texVals[t] = vec3(modalTex((abs(mode) + t) % 8, texcoord));
	
	color = vec4(texVals[0], 1.0F);
	vec3 ambientColor = (lightVal + texVals[(1 + t) % 8]) / 2;
	vec3 ambient = ambientColor * (0.25 + (0.05 * intensity));
	vec3 diffuseColor = (lightVal + texVals[(2 + t) % 8]) / 2;
	vec3 diffuse = diffuseColor * getDiffuse(lightPos - color_range(texVals[(3 + t) % 8]), (target - offset) - color_range(texVals[(4 + t) % 8])) * 0.5 * intensity;
	vec3 specColor = (lightVal + texVals[(5 + t) % 8]) / 2;
	vec3 specular = specColor * getSpecular(lightPos - color_range(texVals[(6 + t) % 8]), vec3(cam_pos), target - color_range(texVals[(7 + t) % 8]), float(1 + intensity));

	color *= vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
