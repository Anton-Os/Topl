#version 440

#extension GL_KHR_vulkan_glsl : enable

// Values

vec3 positions[3] = vec3[](
    vec3(0.0, -0.5, 0.0),
    vec3(0.5, 0.5, 0.0),
    vec3(-0.5, 0.5, 0.0)
);

layout(location = 0) in vec3 pos;
// layout(location = 1) in vec3 texcoord;

// Main

void main() { gl_Position = vec4(pos, 1.0f); }
