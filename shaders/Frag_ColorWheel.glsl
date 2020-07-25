#version 440

uniform sampler2D baseTex; // Why no built-in tex type?

layout(location = 0) in vec2 texcoord;
layout(location = 1) in vec4 hardColor;

out vec4 color;

void main() {
	color = hardColor;
}
