#version 440

// glsl block index 0
layout(packed) uniform Block {
	vec2 rotation; // padded to vec4
	vec3 offset; // padded to vec4
};

// glsl block index 1
layout(packed) uniform SceneBlock {
	vec3 cameraPos; // padded to vec4
	vec3 lookPos; // padded to vec4
	mat4 projMatrix;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 texcoord_out;

mat4 calcCameraMatrix(vec3 cPos, vec3 tPos){
	vec3 defaultUpVec = vec3(0.0, 1.0, 0.0);

	vec3 zAxis = normalize(cPos - tPos);
	vec3 xAxis = normalize(cross(defaultUpVec, zAxis));
	vec3 yAxis = cross(zAxis, xAxis);

	mat4 lookAtMatrix = mat4(
		xAxis.x, xAxis.y, xAxis.z, 0.0,
		yAxis.x, yAxis.y, yAxis.z, 0.0,
		zAxis.x, zAxis.y, zAxis.z, 0.0,
		0.0, 0.0, 0.0, 1.0
	);

	mat4 posMatrix = mat4(
		1.0, 0.0, 0.0, -cPos.x,
		0.0, 1.0, 0.0, -cPos.y,
		0.0, 0.0, 1.0, -cPos.z,
		0.0, 0.0, 0.0, 1.0
	);

	return lookAtMatrix * posMatrix;
}

void main() {
	vec4 finalPos = vec4(pos, 1.0f);
	if (rotation[0] != 0) {
		mat2 zRotMatrix = mat2(
			cos(rotation[0]), sin(rotation[0]),
			-1 * sin(rotation[0]), cos(rotation[0])
		);

		/* mat3 yRotMatrix = mat3(
			// TODO: Implement this
		); */

		vec2 rotCoords = zRotMatrix * vec2(finalPos.x, finalPos.y);
		finalPos.x = rotCoords.x;
		finalPos.y = rotCoords.y;
		finalPos += vec4(offset, 0.0f);
	}

	texcoord_out = texcoord;

	gl_Position = finalPos * calcCameraMatrix(cameraPos, lookPos) * projMatrix;
}