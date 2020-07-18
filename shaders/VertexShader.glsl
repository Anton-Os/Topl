#version 440

layout(std140) uniform Block{
	vec3 offset;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;

void main() {
	texcoord_out = texcoord;

	gl_Position = vec4(pos + offset, 1.0);
}