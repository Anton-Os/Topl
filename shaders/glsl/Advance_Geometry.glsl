#version 440

#define IGNORE_INPUTS
#define INCLUDE_EXTBLOCK

#include "Common.glsl"

/* in gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
} gl_in[];

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
}; */

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

void main(void) {
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.02, 0.05, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.05, -0.02, 0.0, 0.0);
	EmitVertex();

  EndPrimitive();
}
