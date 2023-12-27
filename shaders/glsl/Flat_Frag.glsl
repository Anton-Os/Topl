#version 440

// Values

layout(std140, binding = 0) uniform Block{
	vec4 color;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

layout(std140, binding = 1) uniform SceneBlock{
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};

layout(location = 0) in flat uint vertex_num;
layout(location = 1) in vec3 pos;

layout(location = 0) out vec4 outColor;

// Main

void main() { 
	if (mode == 1){ // alternate mode
		if (vertex_num % 5 == 0) outColor = vec4(0.5f, 0.5f, 1.0f, 0.8f); // color.a); // center vertex
		else if (vertex_num % 5 == 1) outColor = vec4(0.0f, 0.0f, 1.0f, 0.8f); // color.a);
		else if (vertex_num % 5 == 2) outColor = vec4(1.0f, 0.0f, 1.0f, 0.8f); // color.a);
		else if (vertex_num % 5 == 3) outColor = vec4(0.0f, 1.0f, 1.0f, 0.8f); // color.a);
		else if (vertex_num % 5 == 4) outColor = vec4(1.0f, 1.0f, 1.0f, 0.8f); // color.a);
		else outColor = vec4(1.0f, 0.0f, 0.0f, color.a); // red
	} else if (mode == 2) 
		outColor = vec4(pos.x - offset.x + 0.25, pos.y - offset.y + 0.25, pos.z - offset.z + 0.25, color.a); // directional mode
	else outColor = color; // solid mode // default
}
