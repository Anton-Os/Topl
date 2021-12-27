#version 440

layout(location = 0) in vec4 flatColor;

out vec4 color;

void main() {
	color = flatColor;
}
