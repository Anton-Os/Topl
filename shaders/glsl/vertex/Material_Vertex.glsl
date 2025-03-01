#version 440

#define INCLUDE_BLOCK

#include "Common.glsl"

#include "Vertex.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	vec3 texScroll; // texture coordinate scrolling
	vec4 texScale; // texture coordinate scaling

	vec3 lightPos;
	vec3 lightVal;
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) out vec3 vertex_pos_out;
layout(location = 2) out vec3 normal_out;
layout(location = 3) out vec3 texcoord_out;

// Main

void main() {
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));
	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;

	vertex_pos_out = vec3(pos.x, pos.y, pos.z);
	normal_out = getRotMatrix(rotation) * normal_in;
	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	texcoord_out = (texcoord_in + vec3(texScroll)) * vec3(texScale);
#ifdef INCLUDE_EXTBLOCK
	if(gl_InstanceID > 0 && gl_InstanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[gl_InstanceID])) gl_Position *= instanceData[gl_InstanceID];
#endif
}
