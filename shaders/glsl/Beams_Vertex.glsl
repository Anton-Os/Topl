#version 440

// Values

layout(std140, binding = 0) uniform Block {
	// uint renderID
	vec3 offset;
	vec2 rotation;
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

vec3 reflect(vec3 light, vec3 target){
	return light - ((2 * dot(light, normalize(target)) * normalize(target))); 
}

float calcSpec(vec3 light, vec3 target, vec3 camera){
	vec3 reflectVec = reflect(light, target);
	return dot(-1.0 * normalize(camera), reflectVec);
}

float calcDiffuse(vec3 light, vec3 target){
	float intensity = dot(normalize(light), normalize(target));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light)); // length * length is equal to length^2
	// return intensity;
	return intensity * attenuation;
}

float calcAmbient(vec3 light){
	const float attenuation = 0.1;
	return ((light.r * attenuation) + (light.g * attenuation) + (light.b * attenuation)) / 3;
}

mat3 calcRotMatrix(vec2 rotCoords){
	mat3 zRotMatrix = mat3(
		cos(rotCoords.x), -1.0f * sin(rotCoords.x), 0,
		sin(rotCoords.x), cos(rotCoords.x), 0,
		0, 0, 1
	);

	mat3 yRotMatrix = mat3(
		cos(rotCoords.y), 0, -1.0 * sin(rotCoords.y),
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
	vec3 rotCoords = calcRotMatrix(rotation) * pos;
	vec4 final_pos = vec4(rotCoords.x, rotCoords.y, rotCoords.z, 1.0f);

	gl_Position = final_pos + vec4(offset, 0.0f); // * projMatrix;
	// gl_Position = (final_pos + vec4(offset, 0.0f)) * calcCameraMatrix(cam_pos, look_pos) * projMatrix;
	pos_out = vec3(final_pos.x, final_pos.y, final_pos.z);

	// ambient shading
	const float ambient_intensity = 0.25f;
	ambient_out = ambient_intensity * skyLight_value;
	// diffuse shading
	const float skyLight_intensity = calcDiffuse(skyLight_pos, pos);
	diffuse_out = (skyLight_intensity * skyLight_value); // + (flashLight_intensity * flashLight_value);
	// specular shading
	const float specular_intensity = calcSpec(skyLight_pos, pos, cam_pos);
	specular_out = reflect(skyLight_pos, pos); // for testing
}