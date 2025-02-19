#ifdef INCLUDE_BLOCK
layout(std140, binding = 0) uniform Block {
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};
#endif

#ifdef INCLUDE_SCENEBLOCK
layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};
#endif

#ifdef INCLUDE_EXTBLOCK
#define MAX_INSTANCES 3

layout(std140, binding = 2) uniform ExtBlock {
	uint vertCount; // count for vertices
	uint instCount; // count for rendering instances
	uint drawMode; // draw mode cooresponding to primitive
	uint tessLevel; // levels of tesselation

	mat4 instanceData[MAX_INSTANCES];
};
#endif

#ifdef INCLUDE_DATASTREAM

// layout(std140, binding = 3) readonly buffer FeedIn { vec3 data[]; };
layout(std140, binding = 3) writeonly buffer FeedOut { vec3 data[]; };

#endif

#define TRACER_STEPS 16
#define TRACER_PATHS 16

#ifdef INCLUDE_TEXTURES

#ifndef SLICE
#define SLICE 0.0f
#endif

layout(binding = 0) uniform sampler2D baseTex;
layout(binding = 1) uniform sampler2D tex1;
layout(binding = 2) uniform sampler2D tex2;
layout(binding = 3) uniform sampler2D tex3;
layout(binding = 4) uniform sampler2D tex4;
layout(binding = 5) uniform sampler2D tex5;
layout(binding = 6) uniform sampler2D tex6;
layout(binding = 7) uniform sampler2D tex7;
layout(binding = 8) uniform sampler3D volumeTex;

#endif

#define COLOR_INC 0.00390625

// uniform vec4 controlPoints[64];
// uniform vec4 nearestVertex[1024];

#ifndef IGNORE_INPUTS
layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 texcoord_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec3 vert_color_in;
// layout(location = 3) in mat3 instanceData;
#endif

// Functions

uvec4 getModes(int mode){
	return uvec4(abs(mode) % 10, (abs(mode) - (abs(mode) % 10)) / 10, (abs(mode) - (abs(mode) % 100)) / 100, (abs(mode) - (abs(mode) % 1000)) / 1000);
}

float sum(vec3 data){
	return abs(data.x) + abs(data.y) + abs(data.z);
}

bool nonZeroMatrix(mat4 targetMatrix){
	for(uint m = 0; m < 4; m++) 
		for(uint n = 0; n < 4; n++)
			if(targetMatrix[m][n] != 0.0) return true;
	return false;
}