#version 440

// Values

layout(vertices = 3) out;

// Main

void main(void) {
	if (gl_InvocationID == 0) { // only for first invocation
		gl_TessLevelOuter[0] = 0.5f;
		gl_TessLevelOuter[1] = 0.5f;
		gl_TessLevelOuter[2] = 0.5f;
		gl_TessLevelInner[0] = 0.5f;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; // copy input to output
}