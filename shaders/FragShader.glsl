#version 440

uniform sampler2D baseTex; // Why no built-in tex type?

layout(location = 0) in vec2 texcoord;

out vec4 color;

void main() {
	color = texture(baseTex, texcoord);

	/* if (texcoord[0] < 0.5 && texcoord[1] < 0.5)
		color = vec4(0.1, 0.9, 0.22, 1.0);
	else
		color = vec4(0.9, 0.1, 0.22, 1.0); */
}
