#version 440

uniform sampler2D baseTex;

// glsl block index 1
layout(packed) uniform SceneBlock {
	mat4 projMatrix;
};

layout(location = 0) in vec2 texcoord;

out vec4 color;

void main() {
	color = texture(baseTex, texcoord);

	if (color.a < 0.5) discard; // Quick blending fix
}
