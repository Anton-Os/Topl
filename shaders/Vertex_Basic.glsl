#version 440

layout(packed) uniform Block {
	vec2 rotation; // padded to vec4
	vec3 offset; // padded to vec4
};

/* layout(packed) uniform SceneBlock {
	mat4 projMatrix;
}; */

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;

void main() {
	vec3 finalPos = pos;
	if (rotation.x != 0) {
		mat2 rotMatrix = mat2(
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		);

		vec2 rotCoords = rotMatrix * vec2(finalPos.x, finalPos.y);
		finalPos.x = rotCoords.x;
		finalPos.y = rotCoords.y;
	}

	texcoord_out = texcoord;
	gl_Position = vec4(finalPos + vec3(offset), 1.0);
}