#version 440

layout(location = 0) in uint flatColor;

out vec4 color;

void main() {
	color = vec4(0.8f, 0.4f, 0.2f, 1.0f); // TODO: Make this equal to flatColor input
}
