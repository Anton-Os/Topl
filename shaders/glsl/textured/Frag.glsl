#version 440

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

vec4 glass_effect(vec3 texcoord, uint size){
	uint s = (size + 1) * 2;
	vec3 coord = vec3((texcoord.x * s) - floor(texcoord.x * s), (texcoord.y * s) - floor(texcoord.y * s), (texcoord.z * s) - floor(texcoord.z * s));
	vec3 color = getRandColor(uint(floor(texcoord.x * s)) + (uint(floor(texcoord.y * s)) * 10) + (uint(floor(texcoord.z * s)) * 100));

	// return vec4(pow(color.x, coord.x), pow(color.y, coord.y), pow(color.z, coord.z), length(coord - color));
	return vec4(pow(color.x, 1.0 - coord.x), pow(color.y, 1.0 - coord.y), pow(color.z, 1.0 - coord.z), length(coord - color));
}

vec4 dot_effect(vec3 texcoord, uint size){
	uint s = (size + 1) * 2;
	vec3 coord = vec3((texcoord.x * s) - floor(texcoord.x * s), (texcoord.y * s) - floor(texcoord.y * s), (texcoord.z * s) - floor(texcoord.z * s));
	vec3 color = getRandColor(uint(floor(texcoord.x * s)) + (uint(floor(texcoord.y * s)) * 10) + (uint(floor(texcoord.z * s)) * 100));

	if(length(coord - vec3(0.5)) < 0.25) return vec4(pow(color.x, 1.0 - coord.x), pow(color.y, 1.0 - coord.y), pow(color.z, 1.0 - coord.z), length(coord - color));
	else return vec4(0.0, 0.0, 0.0, 0.0);
}

void main() {
#ifdef INCLUDE_TEXTURES
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
#else
	// color_final = vec4(1.0, 0.0, 0.0, 1.0);
	if(mode > 0) color_final = glass_effect(texcoord, uint(abs(mode)));
	else color_final = dot_effect(texcoord, uint(abs(mode)));
#endif
	if (color_final.a < 0.05) discard; // blending fix
}
