#version 440

// layout(location = 0) in vec3 texcoord;
layout(location = 1) in vec4 flatColor;

out vec4 color;

void main() {
	// color = vec4(0.8f, 0.4f, 0.2f, 1.0f);
	color = flatColor;
}
