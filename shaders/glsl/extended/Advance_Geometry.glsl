#version 440

#define STAGE_GEOMETRY

#include "Extended.glsl"

void main(void) {
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.02, 0.05, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.05, -0.02, 0.0, 0.0);
	EmitVertex();

  	EndPrimitive();
}
