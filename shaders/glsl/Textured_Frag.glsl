#version 440

// Values

layout(binding = 0) uniform sampler2D baseTex;
layout(binding = 1) uniform sampler2D tex1; // height?
layout(binding = 2) uniform sampler2D tex2; // roughness?
layout(binding = 3) uniform sampler2D tex3; // opacity?
layout(binding = 4) uniform sampler2D tex4; // enviornment?
layout(binding = 5) uniform sampler2D tex5; // shadow?
layout(binding = 6) uniform sampler3D volumeTex;

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
	// color = texture(volumeTex, vec3(texcoord.x, texcoord.y, texcoord.z));
	color = switchRB(color);

	if (color.a < 0.5) discard; // quick blending fix
}
