#version 440

#define INCLUDE_SCENEBLOCK
#define INCLUDE_TEXTURES

#include "Common.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;

	vec3 texScroll; // texture coordinate scrolling
	// vec3 texScale; // texture coordinate scaling
};

layout(location = 0) out vec3 texcoord_out;
// layout(location = 1) out int mode_out;

// Main

void main() {
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f) * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	// gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;
	if(mode < 0){
		vec4 heightCoord = texture(baseTex, vec2(texcoord.x, texcoord.y));
		gl_Position.x += heightCoord.g * abs(mode);
		gl_Position.y += heightCoord.r * abs(mode);
		gl_Position.z += heightCoord.b * abs(mode);
	}
	
	texcoord_out = texcoord + texScroll;
	// texcoord_out = (texcoord + texScroll) * texScale;
}