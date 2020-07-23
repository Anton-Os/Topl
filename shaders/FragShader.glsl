#version 440

uniform sampler2D baseTex; // Why no built-in tex type?

layout(location = 0) in vec2 texcoord;

out vec4 color;

void main() {
	color = texture(baseTex, texcoord);

	if (color.a < 0.5) discard; // Quick blending fix
}
