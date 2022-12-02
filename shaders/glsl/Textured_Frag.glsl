#version 440

// Values

layout(binding = 0) uniform sampler2D baseTex;
layout(binding = 1) uniform sampler3D areaTex;

layout(location = 0) in vec3 texcoord;

layout(location = 0) out vec4 color;

// Funtions

vec4 switchRB(vec4 color){ // switch red and blue color values
	float t = color.r;
	color.r = color.b;
	color.b = t;
	return color;
}

// Main

void main() {
	color = texture(baseTex, vec2(texcoord.x, texcoord.y));
	color = switchRB(color);

	if (color.a < 0.5) discard; // quick blending fix
}
