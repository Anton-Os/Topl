#version 440

// Values

layout(std140, binding = 1) uniform SceneBlock{
	int mode;
	vec3 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;

	vec3 skyLight_pos; vec3 skyLight_value;
	vec3 flashLight_pos; vec3 flashLight_value;
	vec3 lampLight_pos; vec3 lampLight_value;
};


layout(location = 1) in vec3 pos;

layout(location = 0) out vec4 color;

// Functions

float calcSpec(vec3 light, vec3 camera, vec3 vertex) {
	vec3 reflectVec = light - (normalize(vertex) * 2 * dot(light, normalize(vertex)));
	return max(pow(dot(reflectVec, -normalize(cam_pos)), 3), 0);
}

float calcDiffuse(vec3 light, vec3 vertex) {
	float intensity = dot(normalize(light), normalize(vertex));
	intensity = (intensity + 1.0) * 0.5; // distributes light more evenly
	float attenuation = 1 / (length(light) * length(light));
	return intensity * attenuation;
}

// Main

void main() {
	vec3 ambient = skyLight_value * 0.2;
	vec3 diffuse = skyLight_value * calcDiffuse(skyLight_pos, pos) * 0.5;
	vec3 specular = skyLight_value * calcSpec(skyLight_pos, cam_pos, pos);

	if(mode == 1) color = vec4(ambient, 1.0f);
	else if(mode == 2) color = vec4(diffuse, 1.0f);
	else if(mode == 3) color = vec4(specular, 1.0f);
	else if(mode == 4){ // depth mode
		float depth = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2)); // depth calculation
		color = vec4(depth, depth, depth, 1.0f);
	} 
	else color = vec4(ambient + diffuse + specular, 1.0f); // all lighting
}
