#version 440

#define INCLUDE_TEXTURES
#define INCLUDE_SCENEBLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;

	vec3 texScroll; // texture coordinate scrolling
	vec3 texScale; // texture coordinate scaling
};

layout(location = 0) in vec3 texcoord;

layout(location = 0) out vec4 color;

// Funtions

// Main

void main() {
	if(abs(mode) % 10 == 8) color = color_correct(texture(volumeTex, texcoord)); // volumetric texture
	else if(abs(mode) % 10 == 9) color = color_correct(texture(volumeTex, vec3(texcoord.x, texcoord.y, 0.0))); // volumetric slice
	else { // select texture
		if(abs(mode) % 10 == 1) color = color_correct(texture(tex1, vec2(texcoord.x, texcoord.y)));
		else if(abs(mode) % 10 == 2) color = color_correct(texture(tex2, vec2(texcoord.x, texcoord.y)));
		else if(abs(mode) % 10 == 3) color = color_correct(texture(tex3, vec2(texcoord.x, texcoord.y)));
		else if(abs(mode) % 10 == 4) color = color_correct(texture(tex4, vec2(texcoord.x, texcoord.y)));
		else if(abs(mode) % 10 == 5) color = color_correct(texture(tex5, vec2(texcoord.x, texcoord.y)));
		else if(abs(mode) % 10 == 6) color = color_correct(texture(tex6, vec2(texcoord.x, texcoord.y)));
		else if(abs(mode) % 10 == 7) color = color_correct(texture(tex7, vec2(texcoord.x, texcoord.y)));
		else color = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // base texture
	}

	if (color.a < 0.05) discard; // blending fix
}
