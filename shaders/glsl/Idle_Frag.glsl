#version 440
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Values

layout(location = 0) out vec4 color_out;

// Main

void main() { color_out = vec4(1.0, 1.0, 0.0, 1.0); }
