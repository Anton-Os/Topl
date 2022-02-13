#version 440

layout(location = 0) in vec4 ambient;
layout(location = 1) in vec4 diffuse;
layout(location = 2) in vec4 specular;

out vec4 color;

void main() {
	color = ambient;
}
