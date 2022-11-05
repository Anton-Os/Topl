#version 440

// Values

layout(std140, binding = 0) uniform Block {
	// uint renderID;
	vec4 color;
	vec3 offset;
	vec2 rotation;
};

layout(std140, binding = 1) uniform SceneBlock {
	uint mode;
	vec3 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec4 flatColor_out;

// Functions

mat3 calcRotMatrix(vec2 rotCoords){
	mat3 zRotMatrix = mat3(
		cos(rotCoords.x), -1.0f * sin(rotCoords.x), 0,
		sin(rotCoords.x), cos(rotCoords.x), 0,
		0, 0, 1
	);

	mat3 yRotMatrix = mat3(
		cos(rotCoords.y), 0, -1.0f * sin(rotCoords.y),
		0, 1, 0,
		sin(rotCoords.y), 0, cos(rotCoords.y)
	);

	return zRotMatrix * yRotMatrix;
}

mat4 calcCamMatrix(vec3 cPos, vec2 lPos){  // camera postion and relative look position
	mat4 camMatrix = mat4(
		1.0, 0.0, 0.0, cPos.x,
		0.0, 1.0, 0.0, cPos.y,
		0.0, 0.0, 1.0, cPos.z,
		0.0, 0.0, 0.0, 1.0
	);

	return camMatrix;
} 

// Main

void main() {
	vec3 rotCoords = calcRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(rotCoords.x, rotCoords.y, rotCoords.z, 1.0f);

	// mat4 cameraMatrix = calcCamMatrix(cam_pos, vec2(look_pos.x, look_pos.y));
	mat4 cameraMatrix = calcCamMatrix(vec3(0.1, 0.3, -1.0), vec2(0.0, 0.0)); // camera test
	gl_Position = (final_pos + vec4(offset, 0.0f)) * projMatrix;
	// gl_Position = (final_pos + vec4(offset, 0.0f)) * cameraMatrix * projMatrix;

	if(mode == 1) // alternate mode
		if(gl_VertexID % 4 == 0) flatColor_out = vec4(1.0f, 0.0, 0.0f, color.a); // red
		else if(gl_VertexID % 4 == 1) flatColor_out = vec4(0.0f, 1.0f, 0.0f, color.a); // green
		else if(gl_VertexID % 4 == 2) flatColor_out = vec4(0.0f, 0.0f, 1.0f, color.a); // blue
		else flatColor_out = vec4(0.0f, 0.0f, 0.0f, color.a); // black
	// else if(mode == 2)
	else flatColor_out = color; // solid mode // default
}