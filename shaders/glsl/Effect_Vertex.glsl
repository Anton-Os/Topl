#version 440

layout(packed, binding = 0) uniform Block {
	uint renderID;
};

layout(packed, binding = 1) uniform SceneBlock {
	ivec2 screenRes;
	vec2 cursorPos;
	// uint renderMode; // switches modes to implement different drawing functions
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

// layout(location = 0) flat out uint renderID_out;
layout(location = 0) flat out ivec2 screenRes_out;
layout(location = 1) flat out vec2 cursorPos_out;
layout(location = 2) flat out uint renderID_out;

void main() {
	// renderID_out = renderID;
    screenRes_out = screenRes;
	cursorPos_out = cursorPos; // change to uniform value
	renderID_out = renderID;
	gl_Position = vec4(pos, 1.0);
}