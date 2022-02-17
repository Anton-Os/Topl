#version 440

layout(packed, binding = 0) uniform Block {
	vec3 offset; // padded to vec4
	vec2 rotation; // padded to vec4
};

/* layout(packed, binding = 0) uniform SceneBlock {
	mat4 projMatrix;
}; */

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;

void main() {
	vec4 final_pos = vec4(pos, 1.0f);
	final_pos += vec4(offset, 0.0f);

	gl_Position = final_pos;
	texcoord_out = texcoord;
}