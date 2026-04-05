#version 440

// #define INCLUDE_EXTBLOCK
#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Vertex.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	// uint actorID;
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
	vec2 cursorPos;
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) out vec3 vert_pos_out;
layout(location = 2) out vec3 normal_out;
layout(location = 3) out vec4 vert_color_out;
layout(location = 4) out vec3 texcoord_out;
layout(location = 5) out vec3 tangent_out;
layout(location = 6) flat out int id_out;

void main() {
	float cursorDist = sqrt(pow(cursorPos.x, 2) + pow(cursorPos.y, 2));
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));
	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(cursorPos.x - gl_Position.x, cursorPos.y - gl_Position.y, cursorDist - gl_Position.z);
	vert_pos_out = vec3(cursorPos.x - pos.x, cursorPos.y - pos.y, cursorDist - pos.z); // vec3(pos);
	normal_out = vec3(cursorPos.x - normal_in.x, cursorPos.y - normal_in.y, cursorDist - normal_in.z); // normal_in;
	vert_color_out = vec4(cursorPos.x - vert_color_in.r, cursorPos.y - vert_color_in.g, cursorDist - vert_color_in.b, 1.0);
	texcoord_out = vec3(cursorPos.x - texcoord_in.x, cursorPos.y - texcoord_in.y, cursorDist - texcoord_in.z); // texcoord_in;
	tangent_out = vec3(cursorPos.x - tangent_in.x, cursorPos.y - tangent_in.y, cursorDist - tangent_in.z); // tangent_in;
	id_out = gl_VertexID;
#ifdef INCLUDE_EXTBLOCK
	if(gl_InstanceID > 0 && gl_InstanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[gl_InstanceID])) 
		gl_Position = pos * instanceData[gl_InstanceID] * getCamMatrix(cam_pos, look_pos) * projMatrix;
#endif
}
