#version 440

layout(packed, binding = 0) uniform Block {
	vec3 offset; // padded to vec4
	vec2 rotation; // padded to vec4
	// vec4 color;
};

layout(packed, binding = 1) uniform SceneBlock {
	vec3 lookPos; // padded to vec4
	vec3 cameraPos; // padded to vec4
	mat4 projMatrix;

	vec3 light1_pos; // padded to vec4
	vec3 light1_color; float light1_intensity;

	vec3 light2_pos; // padded to vec4
	vec3 light2_color; float light2_intensity;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec4 ambient_out;
layout(location = 1) out vec4 diffuse_out;
layout(location = 2) out vec4 specular_out;

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
	vec4 finalPos = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 finalTrans = pos + offset;

//	if (rotation[0] != 0 || rotation[1] != 0) {
		mat3 zRotMatrix = mat3(
			cos(rotation[0]), sin(rotation[0]), 0,
			-1 * sin(rotation[0]), cos(rotation[0]), 0,
			0, 0, 1
		);

		mat3 yRotMatrix = mat3(
			cos(rotation[1]), 0, -1 * sin(rotation[1]),
			0, 1, 0,
			sin(rotation[1]), 0, cos(rotation[1])
		);

		vec3 rotCoords = (zRotMatrix * yRotMatrix) * pos;
		// vec3 rotCoords = zRotMatrix * pos; // test case
		finalPos = vec4(rotCoords, 0.0);
//	}

	finalPos += vec4(finalTrans, 1.0);

	gl_Position = finalPos * calcCameraMatrix(cameraPos, lookPos);
	// gl_Position = finalPos * calcCameraMatrix(cameraPos, lookPos) * projMatrix;
	ambient_out = vec4(1.0f, 0.0, 0.0, 1.0f); // red
	diffuse_out = vec4(0.0, 1.0f, 0.0, 1.0f); // green
	specular_out = vec4(0.0, 0.0, 1.0f, 1.0f); // blue
}