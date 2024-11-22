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
layout(location = 1) in vec3 vertex_color;
layout(location = 2) flat in int id;
layout(location = 3) in vec3 texcoord;

layout(location = 0) out vec4 outColor;

// Main

void main() {
	uint primID = gl_PrimitiveID;
	float alpha = 1.0f;
	if(abs(mode) >= 10) alpha -= -pos.z * ceil(mode / 10.0);

	if(mode == -1) // indexing mode
		outColor = getStepColor(id);
	else if(mode < -1){
		uint target = primID;
		while(target > mode * -1) target -= mode * -1;
		outColor = getStepColor(target);
	}
	else if (mode % 10 == 1) // directional mode
		outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, alpha);
	else if (mode % 10 == 2) // coordinate mode
		outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else if (mode % 10 == 3) // vertex mode
    	outColor = vec4(vertex_color, alpha);
	else if(mode % 10 == 4) // camera mode
		outColor = vec4(abs(cam_pos.x - pos.x), abs(cam_pos.y - pos.y), abs(cam_pos.z - pos.z), alpha);
	else if(mode % 10 == 5) // angular mode
		outColor = vec4(atan((pos.y - offset.y) / (pos.x - offset.x)), atan((pos.y - offset.y) / (pos.z - offset.z)), atan((pos.x - offset.x) / (pos.z - offset.z)), alpha);
	// 	outColor = color * vec4(scale.x / (pos.x - offset.x), scale.y / (pos.y - offset.y), scale.z / (pos.z - offset.z), 1.0);
	else if(mode % 10 == 6) // texcoord mode
		outColor = vec4(texcoord.x, texcoord.y, texcoord.z, alpha);
	else if(mode % 10 == 7) // sectioned mode
		outColor = vec4(((id + 1) / 3.0) - floor((id + 1) / 3.0), ((id + 1) / 6.0) - floor((id + 1) / 6.0), ((id + 1) / 9.0) - floor((id + 1) / 9.0), alpha);
	else if(mode % 10 == 8) // randomized mode
		outColor = vec4(getRandColor(primID), alpha);
		// outColor = getRandColor(uint(floor((vertex_color.r * vertex_color.g * vertex_color.b) * 10.0)));
	else if(mode % 10 == 9) // experimental mode
		outColor = vec4(sin(vertex_color.r * id), cos(vertex_color.g * primID), tan(vertex_color.b * (id / primID)), alpha);
	else outColor = color; // solid mode // default
}
