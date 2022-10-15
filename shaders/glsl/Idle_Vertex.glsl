#version 440

// Values

layout(std140, binding = 0) uniform Block{
	uint renderID;
};

layout(std140, binding = 0) uniform SceneBlock{
	uint mode;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

// Main

void main() {
	gl_Position = vec4(pos, 1.0f);
}