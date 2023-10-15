#version 440
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Values

layout(location = 0) out vec4 color;

// Main

void main() {
	color = vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow color test
}
