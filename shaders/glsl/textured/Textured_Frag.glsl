#version 440

#define INCLUDE_TEXTURES
#define INCLUDE_SCENEBLOCK
#define IGNORE_INPUTS

layout(std140, binding = 0) uniform Block {
	// uint actorID;
	vec3 offset;
	vec3 rotation;
	vec3 scale;

	vec3 texScroll; // texture coordinate scrolling
	vec4 texScale; // texture coordinate scaling

	uint flip;
	float antialiasArea;
	uint antialiasSteps;
	float slice;
};

#define SLICE slice

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(location = 0) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

// Main

void main() {
	if(abs(mode) % 10 == 8) color_final = antialias3D(texcoord, volumeTex, antialiasArea, antialiasSteps);
	else if(abs(mode) % 10 == 9) color_final = antialias3D(vec3(texcoord.x, texcoord.y, slice), volumeTex, antialiasArea, antialiasSteps);
	else { // select texture
		if(abs(mode) % 10 == 1) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex1, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 2) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex2, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 3) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex3, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 4) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex4, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 5) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex5, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 6) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex6, antialiasArea, antialiasSteps);
		else if(abs(mode) % 10 == 7) color_final = antialias2D(vec2(texcoord.x, texcoord.y), tex7, antialiasArea, antialiasSteps);
		else color_final = antialias2D(vec2(texcoord.x, texcoord.y), baseTex, antialiasArea, antialiasSteps); // base texture
	}

	if (color_final.a < 0.05) discard; // blending fix
}
