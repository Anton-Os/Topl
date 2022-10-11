#version 440

// Values

layout(std140, binding = 0) uniform Block {
	uint actorID;
};

layout(std140, binding = 1) uniform SceneBlock {
	ivec2 screenRes;
	vec2 cursorPos;
	uint mode;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

// layout(location = 0) flat out uint actorID_out;
layout(location = 0) out ivec2 screenRes_out;
layout(location = 1) out vec2 cursorPos_out;
layout(location = 2) out uint mode_out;
layout(location = 3) flat out uint actorID_out;

// Main

void main() {
	vec2 cursorPosAdj = (cursorPos * 0.5f) + 0.5f;
	
    screenRes_out = screenRes;
	cursorPos_out = cursorPosAdj; // adjusted coordinates
	mode_out = mode;
	actorID_out = actorID;
	gl_Position = vec4(pos, 1.0);
}