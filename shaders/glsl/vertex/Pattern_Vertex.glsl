#version 440

// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Vertex.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	double timeFrame;
	double timeElapse;
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) out vec3 vert_pos_out;
layout(location = 2) out vec3 normal_out;
layout(location = 3) out vec4 vert_color_out;
layout(location = 4) out vec3 texcoord_out;
layout(location = 5) out vec3 tangent_out;
layout(location = 6) flat out int id_out;

void main() {
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));
	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	vert_pos_out = vec3(pos);
	normal_out = normal_in;
	vert_color_out = vec4(vert_color_in, 1.0);
	texcoord_out = texcoord_in;
	tangent_out = tangent_in;
	id_out = gl_VertexID;
#ifdef INCLUDE_EXTBLOCK
	if(gl_InstanceID > 0 && gl_InstanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[gl_InstanceID])) gl_Position *= instanceData[gl_InstanceID];
#endif
}
