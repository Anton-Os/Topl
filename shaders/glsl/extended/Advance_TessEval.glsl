#version 440

#define STAGE_TESS_EVAL

// Values

layout(triangles, equal_spacing, ccw) in;

// Main

void main(void) {
        vec4 tess_x = gl_TessCoord.x * gl_in[0].gl_Position;
        vec4 tess_y = gl_TessCoord.y * gl_in[1].gl_Position;
        vec4 tess_z = gl_TessCoord.z * gl_in[2].gl_Position;

        gl_Position = (tess_x + tess_y + tess_z); // copy input to output
}
