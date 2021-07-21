#version 440

// glsl block index 0
layout(packed) uniform Block {
	vec2 rotation; // padded to vec4
	vec3 offset; // padded to vec4
};

// glsl block index 1
layout(packed) uniform SceneBlock {
	vec3 cameraPos; // padded to vec4
	vec3 cameraRot; // padded to vec4
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;

void main() {
	vec4 finalPos = vec4(pos, 1.0f);
	if (rotation.x != 0) {
		mat2 rotMatrix = mat2(
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		);

		vec2 rotCoords = rotMatrix * vec2(finalPos.x, finalPos.y);
		finalPos.x = rotCoords.x; finalPos.y = rotCoords.y;
	}

	texcoord_out = texcoord;

	mat4 identityMatrix = mat4(1.0); // identity matrix
	gl_Position = finalPos + (vec4(offset, 0.0f) * projMatrix);
}