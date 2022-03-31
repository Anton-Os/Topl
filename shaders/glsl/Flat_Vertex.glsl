#version 440

layout(std140, binding = 0) uniform Block {
	uint mode;
	vec3 offset;
	vec2 rotation; // padded to vec4
	vec4 color;
};

layout(std140, binding = 1) uniform SceneBlock {
	vec3 look_pos; // padded to vec4
	vec3 cam_pos; // padded to vec4
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec4 flatColor_out;

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

void main() {
	vec4 final_pos = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 final_trans = pos + offset;

	vec3 rotCoords = calcRotMatrix(vec2(rotation.x, rotation.y)) * pos;
	final_pos = vec4(rotCoords, 0.0) + vec4(final_trans, 1.0); // rotation and translation

	// gl_Position = final_pos * calcCameraMatrix(cam_pos, look_pos);
	gl_Position = final_pos * calcCameraMatrix(cam_pos, look_pos) * projMatrix;

	if(mode == 0) flatColor_out = color; // default mode
	else if(mode == 1) // alternate mode
		if(gl_VertexID % 9 == 0 || gl_VertexID % 9 == 1 || gl_VertexID % 9 == 2 ) 
			flatColor_out = vec4(color.r, 0.0, 0.0f, color.a); // red
		else if(gl_VertexID % 9 == 3 || gl_VertexID % 9 == 4 || gl_VertexID % 9 == 5 ) 
			flatColor_out = vec4(0.0f, color.g, 0.0f, color.a); // green
		else 
			flatColor_out = vec4(0.0f, 0.0f, color.b, color.a); // blue

	else flatColor_out = vec4(1.0f, 0.0f, 0.0f, 1.0f); // mode not supported!
}