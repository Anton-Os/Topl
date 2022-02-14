#version 440

layout(packed, binding = 0) uniform Block {
	uint actorID;
};

layout(packed, binding = 1) uniform SceneBlock {
	ivec2 screenRes;
	vec2 cursorPos;
	// uint renderMode; // switches modes to implement different drawing functions
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

// layout(location = 0) flat out uint actorID_out;
layout(location = 0) flat out ivec2 screenRes_out;
layout(location = 1) flat out vec2 cursorPos_out;
layout(location = 2) flat out uint actorID_out;

void main() {
	vec2 cursorPosAdj = (cursorPos * 0.5f) + 0.5f;
	
    screenRes_out = screenRes;
	cursorPos_out = cursorPosAdj; // adjusted coordinates
	actorID_out = actorID;
	gl_Position = vec4(pos, 1.0);
}