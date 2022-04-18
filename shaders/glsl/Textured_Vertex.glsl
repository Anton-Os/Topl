#version 440

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec2 rotation;
};

layout(std140, binding = 0) uniform SceneBlock {
	vec3 look_pos;
	vec3 cam_pos;
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;

void main() {
	vec4 final_pos = vec4(pos, 1.0f);

	// Rotation Operation
	if (rotation.x != 0) {
		mat2 rotMatrix = mat2(
			cos(rotation.x), -1.0f * sin(rotation.x),
			sin(rotation.x), cos(rotation.x)
		);

		vec2 rotCoords = rotMatrix * vec2(final_pos.x, final_pos.y);
		final_pos.x = rotCoords.x; final_pos.y = rotCoords.y;
	}

	gl_Position = final_pos + (vec4(offset, 0.0f) * projMatrix);
	texcoord_out = texcoord;
}