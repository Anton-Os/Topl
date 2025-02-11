#version 440

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

#define GEOM_INPUT layout(points) // TODO: Define this inside Advance_Shader.hpp
#define GEOM_OUTPUT layout (triangle_strip, max_vertices = 3) // TODO: Define this inside Advance_Shader.hpp

GEOM_INPUT in;
GEOM_OUTPUT out;

void main(void) {
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.02, 0.05, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.05, -0.02, 0.0, 0.0);
	EmitVertex();

  EndPrimitive();
}
