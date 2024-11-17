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

	if(mode < -1){
		uint target = primID;
		while(target > mode * -1) target -= mode * -1;
		outColor = getStepColor(target);
	}
	else if (mode == 1) // directional mode
		outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, color.a);
	else if (mode == 2) // coordinate mode
		outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if (mode == 3) // random color mode
		outColor = getRandColor(color);
	else if (mode == 4) // vertex mode
    	outColor = vertex_color;
	else if (mode == 5) // scale mode
	 	outColor = color * vec4(scale.x / (pos.x - offset.x), scale.y / (pos.y - offset.y), scale.z / (pos.z - offset.z), 1.0);
	else if(mode == 6) // id mode
		outColor = vec4(((id + 1) / 3.0) - floor((id + 1) / 3.0), ((id + 1) / 6.0) - floor((id + 1) / 6.0), ((id + 1) / 9.0) - floor((id + 1) / 9.0), 1.0);
	else if(mode == 7) // camera mode
		outColor = vec4(abs(cam_pos.x - offset.x), abs(cam_pos.y - offset.y), abs(cam_pos.z - offset.z), 1.0);
	else if(mode == 8) // dope mode
		outColor = vec4(sin(vertex_color.r * id), cos(color.g * primID), tan(color.b * vertex_color.b * id * primID), 1.0);
	else if(mode == -1) // indexing mode
		outColor = getStepColor(id);
	else outColor = color; // solid mode // default
}
