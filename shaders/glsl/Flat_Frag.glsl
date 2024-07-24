#version 440

#define IGNORE_INPUTS
#define INCLUDE_SCENEBLOCK

#include "Common.glsl"

// Values

layout(std140, binding = 0) uniform Block{
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) flat in int id;

layout(location = 0) out vec4 outColor;

// Main

void main() {
	uint primID = gl_PrimitiveID;

	if (mode == 1) // directional mode
		outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, color.a);
	else if (mode == 2) // coordinate mode
		outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if (mode == 3) // random color mode
		outColor = getRandColor(color);
	else if (mode == 4) // vertex mode
    	outColor = vertex_color;
    // else if (mode == 5)
    //	outColor = vec4(abs(scale.x - offset.x) - pos.x, abs(scale.y - offset.y) - pos.y, abs(scale.z - offset.z) - pos.z, color.a);
    // else if (mode == 6)
    //    outColor = vec4((color.r * id) - floor(color.r * id), (color.g * id) - floor(color.g * id), (color.b * id) - floor(color.b * id), color.a);
	else if(mode == -1) // indexing mode
		outColor = getUniqueColor(primID);
	else if(mode < -1){
		float fraction = float(primID) / float(-mode);
		float level = 1.0 / float(-mode);

		/* uint i = 0;
		for(float s = 0; s < 1.0; s += level){
			if(fraction - floor(fraction) < s) outColor = getUniqueColor(i);
			else break;
			i++;
		} */

		if(fraction - floor(fraction) < 0.25) outColor = vec4(1.0, 0.0, 0.0, 0.8);
		else if(fraction - floor(fraction) < 0.5) outColor = vec4(0.0, 1.0, 0.0, 0.8);
		else if(fraction - floor(fraction) < 0.75) outColor = vec4(0.0, 0.0, 1.0, 0.8);
		else outColor = vec4(1.0, 1.0, 1.0, 0.8);
	}
	else outColor = color; // solid mode // default
}
