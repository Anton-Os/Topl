#version 440

#define STAGE_TESS_CTRL

// Values

layout(vertices = 3) out;

// Main

void main(void) {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; // copy input to output

    if (gl_InvocationID == 0) { // only for first invocation
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 1.0;
        gl_TessLevelOuter[2] = 1.0;
        gl_TessLevelOuter[3] = 1.0;

        gl_TessLevelInner[0] = 1.0;
        gl_TessLevelInner[1] = 1.0;
    }
}
