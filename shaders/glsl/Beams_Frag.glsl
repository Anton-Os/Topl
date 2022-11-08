#version 440

layout(std140, binding = 1) uniform SceneBlock{
	uint mode;
	vec3 look_pos;
	vec3 cam_pos;
	// mat4 projMatrix;

	vec3 skyLight_pos; vec3 skyLight_value;
	vec3 flashLight_pos; vec3 flashLight_value;
	vec3 lampLight_pos; vec3 lampLight_value;
};

// Values

layout(location = 1) in vec3 pos;
layout(location = 2) in vec3 ambient;
layout(location = 3) in vec3 diffuse;
layout(location = 4) in vec3 specular;

out vec4 color;

// Main

void main() {
	if(mode == 1){ // alternate mode
		vec3 light_color = ambient; // ambient test
		// vec3 light_color = diffuse; // diffuse test
		// vec3 light_color = specular; // specular test
		color = vec4(light_color, 1.0f);
	} else if(mode == 2){ // depth mode
		float depth = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2)); // depth calculation
		color = vec4(depth, depth, depth, 1.0f);
	} else color = vec4(ambient + diffuse + specular, 1.0f); // light mode // default
}
