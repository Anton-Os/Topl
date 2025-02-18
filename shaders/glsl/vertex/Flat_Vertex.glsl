#version 440

#define INCLUDE_INPUTS
#define INCLUDE_SCENEBLOCK
// #define INCLUDE_EXTBLOCK

#include "Common.glsl"

#include "Vertex.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(location = 0) out vec3 pos_out;
layout(location = 1) out vec4 vert_color_out;
layout(location = 2) flat out int id_out;
layout(location = 3) out vec3 texcoord_out;

// Main

void main() {
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));
	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	id_out = gl_VertexID;
	texcoord_out = texcoord_in;
	if(mode < 10 || gl_VertexID == mode) vert_color_out = vec4(vert_color_in, 1.0f); // getRandColor(color - (color / (gl_VertexID + 1))); // getStepColor(gl_VertexID);
	else vert_color_out = vec4(0.0F, 0.0f, 0.0F, 0.1F); 
}