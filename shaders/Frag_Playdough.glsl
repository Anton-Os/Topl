#version 440

layout(location = 0) in vec3 flatColor;

out vec4 color;

void main() {
	color = vec4(flatColor, 1.0f);
}
