#version 440

// Values

layout(std140, binding = 0) uniform Block {
	// uint actorID
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

mat4 calcCameraMatrix(vec3 cPos, vec3 lPos){
	vec3 defaultUpVec = vec3(0.0, 1.0, 0.0);

	vec3 zAxis = normalize(cPos - lPos);
	vec3 xAxis = normalize(cross(defaultUpVec, zAxis));
	vec3 yAxis = cross(zAxis, xAxis);

	mat4 camMatrix = mat4(
		xAxis.x, xAxis.y, xAxis.z, -1.0 * dot(xAxis, cPos),
		yAxis.x, yAxis.y, yAxis.z, -1.0 * dot(yAxis, cPos),
		zAxis.x, zAxis.y, zAxis.z, -1.0 * dot(zAxis, cPos),
		0.0, 0.0, 0.0, 1.0
	);

	return camMatrix;
} 

// Main

void main() {
	vec3 transCoords = pos + offset;
	vec3 rotCoords = calcRotMatrix(vec2(rotation.x, rotation.y)) * pos;
	vec4 final_pos = vec4(rotCoords, 0.0) + vec4(transCoords, 1.0); // rotation and translation

	gl_Position = final_pos * calcCameraMatrix(cam_pos, look_pos); // no projection
	// gl_Position = final_pos * calcCameraMatrix(cam_pos, look_pos) * projMatrix;

	if(mode == 1) // alternate mode
		if(gl_VertexID % 4 == 0) flatColor_out = vec4(1.0f, 0.0, 0.0f, color.a); // red
		else if(gl_VertexID % 4 == 1) flatColor_out = vec4(0.0f, 1.0f, 0.0f, color.a); // green
		else if(gl_VertexID % 4 == 2) flatColor_out = vec4(0.0f, 0.0f, 1.0f, color.a); // blue
		else flatColor_out = vec4(0.0f, 0.0f, 0.0f, color.a); // black
	// else if(mode == 2)
	else flatColor_out = color; // solid mode // default
}