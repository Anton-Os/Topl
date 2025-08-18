#version 440

#define INCLUDE_SCENEBLOCK
#define INCLUDE_TEXTURES

#include "Common.glsl"

#include "Vertex.glsl"

// Values

layout(std140, binding = 0) uniform Block {
	// uint actorID;
	vec3 offset;
	vec3 rotation;
	vec3 scale;

	vec3 texScroll; // texture coordinate scrolling
	vec4 texScale; // texture coordinate scaling

	uint flip;
	float antialiasArea;
	uint antialiasSteps;
	float slice;
};

layout(location = 0) out vec3 texcoord_out;
// layout(location = 1) out int mode_out;

// Main

void main() {
	vec4 pos = getVertex(pos_in, offset, rotation, vec4(scale, 1.0 / cam_pos.w));

	texcoord_out = texcoord_in;

	if(flip % 4 != 0){ // performing flip operation
		if(flip % 4 != 1) texcoord_out.x = 1.0 - texcoord_out.x;
		if(flip % 4 != 3) texcoord_out.y = 1.0 - texcoord_out.y;
	}

	texcoord_out = (texcoord_out + texScroll) * vec3(texScale);
	
	if(abs(mode) >= 10){
		vec4 texOffset = texture(baseTex, vec2(texcoord_out.x, texcoord_out.y));
		if(uint(floor(abs(mode) / 10)) % 10 == 1) texOffset = texture(tex1, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 2) texOffset = texture(tex2, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 3) texOffset = texture(tex3, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 4) texOffset = texture(tex4, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 5) texOffset = texture(tex5, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 6) texOffset = texture(tex6, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 7) texOffset = texture(tex7, vec2(texcoord_out.x, texcoord_out.y));
		else if(uint(floor(abs(mode) / 10)) % 10 == 8) texOffset = texture(volumeTex, texcoord_out);

		if(mode >= 0){
			texcoord_out.x += (texOffset.g - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			texcoord_out.y += (texOffset.r - 0.5F) * (floor(abs(mode) / 100.0) + 1);
			texcoord_out.z += (texOffset.b - 0.5F) * (floor(abs(mode) / 100.0) + 1);
		} else {
			pos.x *= texOffset.g * (floor(abs(mode) / 100.0) + 1);
			pos.y *= texOffset.r * (floor(abs(mode) / 100.0) + 1);
			pos.z *= texOffset.b * (floor(abs(mode) / 100.0) + 1);
		}
	}

	gl_Position = pos * getCamMatrix(cam_pos, look_pos) * projMatrix;
#ifdef INCLUDE_EXTBLOCK
	if(gl_InstanceID > 0 && gl_InstanceID < MAX_INSTANCES) if(nonZeroMatrix(instanceData[gl_InstanceID])) gl_Position *= instanceData[gl_InstanceID];
#endif
}
