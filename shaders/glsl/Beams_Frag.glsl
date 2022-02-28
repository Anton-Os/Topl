#version 440

layout(location = 0) in vec3 ambient;
layout(location = 1) in vec3 diffuse;
layout(location = 2) in vec3 specular;
layout(location = 3) in vec4 lampShine; // shine based on distance

out vec4 color;

void main() {
	// vec3 light_color = ambient + diffuse + specular;
	vec3 light_color = specular;
	color = vec4(light_color, 1.0f);
}
