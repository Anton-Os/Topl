#version 440

uniform sampler2D baseTex;
// uniform sampler3D areaTex;

layout(location = 0) in vec2 texcoord;

out vec4 color;

// switch red and blue color values
vec4 switchRB(vec4 color){
	float t = color.r;
	color.r = color.b;
	color.b = t;
	return color;
}

void main() {
	color = texture(baseTex, texcoord);
	color = switchRB(color);

	if (color.a < 0.5) discard; // Quick blending fix
}
