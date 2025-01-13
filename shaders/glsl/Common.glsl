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

vec4 color_correct(vec4 color){ // switch red and blue color values
    float t = color.r;
    color.r = color.b; color.b = t;
    return color;
}

vec3 color_range(vec3 color){ // maps color to +- range
	return (color - vec3(0.5F, 0.5F, 0.5F)) * 2;
}

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

vec4 sampleTexAt(int m, vec3 texcoord){
    vec4 color;
    if(abs(m) % 10 == 8) color = color_correct(texture(volumeTex, texcoord)); // volumetric texture
    else if(abs(m) % 10 == 9) color = color_correct(texture(volumeTex, vec3(texcoord.x, texcoord.y, SLICE))); // volumetric slice
    else { // select texture
        if(abs(m) % 10 == 1) color = color_correct(texture(tex1, vec2(texcoord.x, texcoord.y)));
        else if(abs(m) % 10 == 2) color = color_correct(texture(tex2, vec2(texcoord.x, texcoord.y)));
        else if(abs(m) % 10 == 3) color = color_correct(texture(tex3, vec2(texcoord.x, texcoord.y)));
        else if(abs(m) % 10 == 4) color = color_correct(texture(tex4, vec2(texcoord.x, texcoord.y)));
        else if(abs(m) % 10 == 5) color = color_correct(texture(tex5, vec2(texcoord.x, texcoord.y)));
        else if(abs(m) % 10 == 6) color = color_correct(texture(tex6, vec2(texcoord.x, texcoord.y)));
        else if(abs(m) % 10 == 7) color = color_correct(texture(tex7, vec2(texcoord.x, texcoord.y)));
        else color = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // base texture
    }
    return color;
}


vec4 sampleTex_antialias2D(vec2 coords, sampler2D tex2D, float antialiasArea, float antialiasSteps){
    if(antialiasArea == 0.0 || antialiasSteps == 0) return color_correct(texture(tex2D, coords));
    else { // antialiasing algorithm
        vec4 texColor = color_correct(texture(tex2D, coords));
        for(uint a = 0; a < antialiasSteps; a++){
            float f = (antialiasArea / antialiasSteps) * (a + 1);
            vec4 nebrTexColors[8] = {
                color_correct(texture(tex2D, coords + vec2(f, 0.0))), color_correct(texture(tex2D, coords + vec2(-f, 0.0))), // left and right
                color_correct(texture(tex2D, coords + vec2(0.0, f))), color_correct(texture(tex2D, coords + vec2(0.0, -f))), // top and bottom
                color_correct(texture(tex2D, coords + vec2(f, f))), color_correct(texture(tex2D, coords + vec2(-f, -f))), // top right and bottom left
                color_correct(texture(tex2D, coords + vec2(-f, f))), color_correct(texture(tex2D, coords + vec2(f, -f))) // top left and bottom right
            };
            for(uint n = 0; n < 8; n++) texColor += nebrTexColors[n]; // total
            texColor *= 1.0 / 8; // average
        }
        return texColor;
    }
}

vec4 sampleTex_antialias3D(vec3 coords, sampler3D tex3D, float antialiasArea, float antialiasSteps){
    if(antialiasArea == 0.0 || antialiasSteps == 0) return color_correct(texture(tex3D, coords));
    else {
        vec4 texColor = color_correct(texture(tex3D, coords));
        for(uint a = 0; a < antialiasSteps; a++){
            float f = (antialiasArea / antialiasSteps) * (a + 1);
            for(uint l = 0; l < 3; l++){
                float d = -f + (f * l);
                vec4 nebrTexColors[9] = {
                    color_correct(texture(tex3D, coords + vec3(0.0, 0.0, d))),
                    color_correct(texture(tex3D, coords + vec3(f, 0.0, d))), color_correct(texture(tex3D, coords + vec3(-f, 0.0, d))), // left and right
                    color_correct(texture(tex3D, coords + vec3(0.0, f, d))), color_correct(texture(tex3D, coords + vec3(0.0, -f, d))), // top and bottom
                    color_correct(texture(tex3D, coords + vec3(f, f, d))), color_correct(texture(tex3D, coords + vec3(-f, -f, d))), // top right and bottom left
                    color_correct(texture(tex3D, coords + vec3(-f, f, d))), color_correct(texture(tex3D, coords + vec3(f, -f, d))) // top left and bottom right
                };
                for(uint n = 0; n < 9; n++) texColor += nebrTexColors[n]; // total
                    texColor *= 1.0 / 9; // average
            }
        }
        return texColor;
    }
}
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

mat4 getLookAtMatrix(vec3 cPos, vec3 lPos, vec3 upPos){
	vec3 forward = normalize(lPos - cPos);
	vec3 right = normalize(cross(forward, upPos));
	vec3 up = normalize(cross(right, upPos));
	
	mat4 lookAtMatrix = mat4(
		right.x, right.y, right.z, -cPos.x,
		up.x, up.y, up.z, -cPos.y,
		-forward.x, -forward.y, -forward.z, -cPos.z,
		0, 0, 0, 1
	);

	return lookAtMatrix;
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


// TODO: Include other helper functions
