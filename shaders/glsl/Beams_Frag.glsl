#version 440

layout(location = 0) flat in uint mode;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec3 ambient;
layout(location = 3) in vec3 diffuse;
layout(location = 4) in vec3 specular;

out vec4 color;

void main() {
	if(mode == 0) // light mode
		color = vec4(ambient + diffuse, 1.0f);
	else if(mode == 1){ // alternate mode
		vec3 light_color = specular;
		color = vec4(light_color, 1.0f);
	} else if(mode == 2){ // depth mode
		float depth = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2)); // depth calculation
		color = vec4(depth, depth, depth, 1.0f);
	} else color = vec4(1.0f, 0.0f, 0.0f, 1.0f); // mode not supported
}
