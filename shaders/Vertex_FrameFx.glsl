#version 440

// glsl block index 0
layout(packed) uniform Block {
	uint renderID;
};

// glsl block index 1
layout(packed) uniform SceneBlock {
	uvec2 screenRes;
	uvec2 cursorPos;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out uvec2 screenRes_out;
layout(location = 1) out uvec2 cursorPos_out;

void main() {
    screenRes_out = screenRes;
	cursorPos_out = cursorPos;
	gl_Position = vec4(finalPos, 1.0);
}