#version 440

// Values

layout(std140, binding = 0) uniform Block{
	vec4 color;
	vec3 offset;
	vec3 rotation;
};

layout(std140, binding = 1) uniform SceneBlock{
	uint mode;
	vec3 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};

layout(location = 0) in flat uint vertexNo;
layout(location = 1) in vec3 pos;

layout(location = 0) out vec4 outColor;

// Main

void main() { 
	if (mode == 1) // alternate mode
		if (vertexNo % 4 == 0) outColor = vec4(1.0f, 0.0, 0.0f, color.a); // red
		else if (vertexNo % 4 == 1) outColor = vec4(0.0f, 1.0f, 0.0f, color.a); // green
		else if (vertexNo % 4 == 2) outColor = vec4(0.0f, 0.0f, 1.0f, color.a); // blue
		else outColor = vec4(0.0f, 0.0f, 0.0f, color.a); // black
	else if (mode == 2) { // directional mode
		if (pos.z > 0) outColor = vec4(0, 1, 0, 0.8);
		else if (pos.z < 0) outColor = vec4(0, 0, 1, 0.8);
		else outColor = vec4(1, 0, 0, 0.8);
	}
	else outColor = color; // solid mode // default
}
