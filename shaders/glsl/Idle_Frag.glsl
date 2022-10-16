#version 440

// Values

layout(std140, binding = 0) uniform Block{
	uint renderID;
};

layout(std140, binding = 0) uniform SceneBlock{
	uint mode;
};

out vec4 color;

// Main

void main() {
	color = vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow color test
}
