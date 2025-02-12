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
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f); // * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);
	// final_pos = final_pos * vec4(1.0f + cos(float(timeElapse)), 1.0f + cos(float(timeElapse)), 1.0f + cos(float(timeElapse)), 1.0f);

	// gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;

	pos_out = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	id_out = gl_VertexID;
	texcoord_out = texcoord;
	if(mode < 10) vert_color_out = vec4(vert_color, 1.0f); // getRandColor(color - (color / (gl_VertexID + 1))); // getStepColor(gl_VertexID);
	else if(gl_VertexID == mode) vert_color_out = vec4(1.0F, 1.0f, 1.0F, 1.0F);
	else vert_color_out = vec4(0.0F, 0.0f, 0.0F, 0.1F); 
}