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

layout(location = 0) in vec3 pos;

layout(location = 0) out vec4 outColor;

// Main

void main() { 
	if (mode == 1) // directional
		outColor = vec4((pos.x / 2.0) + 0.5, (pos.y / 2.0) + 0.5, (pos.z / 2.0) + 0.5, color.a);
	else if (mode == 2) // coordinate mode
		outColor = vec4((pos.x - offset.x + cam_pos.x) * 2.0 + 0.5, (pos.y - offset.y + cam_pos.y) * 2.0 + 0.5, (pos.z - offset.z) * 2.0 + 0.5, color.a);
	else outColor = color; // solid mode // default
}
