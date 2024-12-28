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
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 texcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 vert_color;
// layout(location = 3) in mat3 instanceData;
#endif

// Functions

uvec4 getModes(int mode){
	return uvec4(abs(mode) % 10, (abs(mode) - (abs(mode) % 10)) / 10, (abs(mode) - (abs(mode) % 100)) / 100, (abs(mode) - (abs(mode) % 1000)) / 1000);
}

/* vec4 getRandColor(vec4 seedColor){
	vec4 randColor = seedColor * vec4(34.234, 11.559, 81.344, 56.34);

	while(randColor.x > 1.0) randColor.r -= pow(randColor.x, 0.5);213
	while(randColor.y > 1.0) randColor.g -= pow(randColor.y, 0.5);
	while(randColor.z > 1.0) randColor.b -= pow(randColor.z, 0.5);
	while(randColor.a > 1.0) randColor.a -= pow(randColor.a, 0.5);

	return randColor;
} */

vec3 getRandColor(uint seed){
	dvec3 randColor = dvec3(double(seed)* 0.325243, double(seed) * 0.953254, double(seed) * 0.563445);

	for(uint iter = 0; iter < 1; iter++){
		randColor.x += randColor.y * 1.634923;
		randColor.y += randColor.z * 2.456431;
		randColor.z += randColor.x * 3.123145;
	}

	randColor = dvec3(randColor.x - floor(randColor.x), randColor.y - floor(randColor.y), randColor.z - floor(randColor.z));
	return vec3(float(randColor.x), float(randColor.y), float(randColor.z));
}

vec4 getStepColor(uint index){
	float attenuation = floor(index / 6.0) * COLOR_INC;

	if(index % 6 == 0) return vec4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (index % 6 == 1) return vec4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (index % 6 == 2) return vec4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (index % 6 == 3) return vec4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (index % 6 == 4) return vec4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (index % 6 == 5) return vec4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return vec4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}

mat3 getRotMatrix(vec3 angles) {
	mat3 zRotMatrix = mat3( // Roll
		cos(angles.x), sin(angles.x), 0,
		-sin(angles.x), cos(angles.x), 0,
		0, 0, 1
	);

	mat3 xRotMatrix = mat3( // Pitch
		1, 0, 0,
		0, cos(angles.y), sin(angles.y),
		0, -sin(angles.y), cos(angles.y)
	);

	mat3 yRotMatrix = mat3( // Yaw
		cos(angles.z), 0, sin(angles.z),
		0, 1, 0,
		-1.0 * sin(angles.z), 0, cos(angles.z)
	);

	return zRotMatrix * yRotMatrix * xRotMatrix;
}

mat4 getCamMatrix(vec4 cPos, vec3 angles) { // placeholder camera
	return mat4(
		cos(angles.z) * cos(angles.x), -sin(angles.x), sin(angles.z), -cPos.x,
		sin(angles.x), cos(angles.x) * cos(angles.y), sin(angles.y), -cPos.y,
		-1.0 * sin(angles.z), -sin(angles.y), cos(angles.y) * cos(angles.z), -cPos.z,
		0, 0, 0, 1
	);
}

float getLineDistance(vec2 coord, vec2 p1, vec2 p2){
	return abs(((p2.y - p1.y) * coord.x) - ((p2.x - p1.x) * coord.y) + (p2.x * p1.y) - (p2.y * p1.x)) / sqrt(pow(p2.y - p1.y, 2.0) + pow(p2.x - p1.x, 2.0));
}

vec3 getCoordDistances(vec2 coord, vec2 p1, vec2 p2){
	return vec3(
		sqrt(pow(p2.x - p1.x, 2.0) + pow(p2.y - p1.y, 2.0)), // distance between points 1 and 2
		sqrt(pow(coord.x - p1.x, 2.0) + pow(coord.y - p1.y, 2.0)), // distance between coordinate and point 1 
		sqrt(pow(coord.x - p2.x, 2.0) + pow(coord.y - p2.y, 2.0)) // distance between coordinate and point 2
	);
}

vec4 color_correct(vec4 color){ // switch red and blue color values
	float t = color.r;
	color.r = color.b; color.b = t;
	return color;
}

// TODO: Include other helper functions
