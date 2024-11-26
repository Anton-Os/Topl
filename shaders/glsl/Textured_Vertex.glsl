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
	vec3 texScale; // texture coordinate scaling
};

layout(location = 0) out vec3 texcoord_out;
// layout(location = 1) out int mode_out;

// Main

void main() {
	vec3 angles = getRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f) * vec4(scale.x, scale.y, scale.z, 1.0 / cam_pos.w);

	texcoord_out = (texcoord + texScroll) * texScale;
	
	if(abs(mode) >= 10){
		vec4 texOffset = texture(baseTex, vec2(texcoord_out.x, texcoord_out.y));
		if(uint(floor(abs(mode) / 10)) % 10 == 1) texOffset = texture(tex1, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 2) texOffset = texture(tex2, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 3) texOffset = texture(tex3, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 4) texOffset = texture(tex4, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 5) texOffset = texture(tex5, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 6) texOffset = texture(tex6, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 7) texOffset = texture(tex7, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 8) texOffset = texture(volumeTex, texcoord);

		if(mode >= 0){
			texcoord_out.x += (texOffset.g - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			texcoord_out.y += (texOffset.r - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			texcoord_out.z += (texOffset.b - 0.5F) * (floor(abs(mode) / 100.0) + 1);
		} else {
			final_pos.x += (texOffset.g - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			final_pos.y += (texOffset.r - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			final_pos.z += (texOffset.b - 0.5F) * (floor(abs(mode) / 100.0) + 1);
		}
	}

	gl_Position = (final_pos + vec4(offset, 0.0f)) * getCamMatrix(cam_pos, look_pos) * projMatrix;
}