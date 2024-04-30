#version 440
// #extension GL_ARB_separate_shader_objects : enable
// #extension GL_ARB_shading_language_420pack : enable

// Values

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

// Main

void main() { gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f); }