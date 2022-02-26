#version 440

uniform sampler2D baseTex;
// uniform sampler1D lineTex;
uniform sampler3D areaTex;

layout(location = 0) in vec2 texcoord; // may need to increase this to vec3

out vec4 color;

void main() {
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f); // white color test
}
