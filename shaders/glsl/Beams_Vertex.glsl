#version 440

// Values

layout(std140, binding = 0) uniform Block {
	// uint renderID
	vec3 offset;
	vec3 rotation;
};

layout(std140, binding = 1) uniform SceneBlock {
	uint mode;
	vec3 look_pos;
	vec3 cam_pos;
	// mat4 projMatrix;

	vec3 skyLight_pos; vec3 skyLight_value;
	vec3 flashLight_pos; vec3 flashLight_value;
	vec3 lampLight_pos; vec3 lampLight_value;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 1) out vec3 pos_out;
layout(location = 2) out vec3 ambient_out;
layout(location = 3) out vec3 diffuse_out;
layout(location = 4) out vec3 specular_out;

// Functions

float calcSpec(vec3 light, vec3 target, vec3 camera){
	vec3 reflectVec = light - ((2 * dot(light, normalize(target)) * normalize(target))); // reflection vefctor
	return dot(-1.0 * normalize(camera), reflectVec);
}

float calcDiffuse(vec3 light, vec3 target){
	float intensity = dot(normalize(light), normalize(target));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

mat3 calcRotMatrix(vec3 angles) {
	mat3 zRotMatrix = mat3( // Roll
		cos(angles.x), -sin(angles.x), 0,
		sin(angles.x), cos(angles.x), 0,
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
	vec3 angles = calcRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(angles.x, angles.y, angles.z, 1.0f);

	gl_Position = final_pos + vec4(offset, 0.0f); // * projMatrix;
	// gl_Position = (final_pos + vec4(offset, 0.0f)) * calcCameraMatrix(cam_pos, look_pos) * projMatrix;
	pos_out = vec3(final_pos.x, final_pos.y, final_pos.z);

	// ambient shading
	const float ambient_intensity = 0.25f;
	ambient_out = ambient_intensity * skyLight_value;
	// diffuse shading
	const float skyLight_intensity = calcDiffuse(skyLight_pos, angles);
	diffuse_out = skyLight_intensity * skyLight_value; // + (flashLight_intensity * flashLight_value);
	// specular shading
	const float specular_intensity = calcSpec(skyLight_pos, angles, cam_pos);
	specular_out = specular_intensity * skyLight_value; // reflect(skyLight_pos, pos); // for testing
}