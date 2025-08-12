#version 440

#extension GL_KHR_vulkan_glsl : enable

#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

vec3 positions[3] = vec3[](
    vec3(0.0, -0.5, 0.0),
    vec3(0.5, 0.5, 0.0),
    vec3(-0.5, 0.5, 0.0)
);

layout(location = 0) in vec3 pos;
// layout(location = 1) in vec3 texcoord;

// Main

void main() { 
    gl_Position = vec4(pos, 1.0f);

    if(gl_InstanceID >= 1){
        if(gl_InstanceID % 5 == 1) gl_Position = gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
        else if(gl_InstanceID % 5 == 2) gl_Position = gl_Position + vec4(-0.1, -0.1, 0.0, 0.0);
        else if(gl_InstanceID % 5 == 3) gl_Position = gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);
        else if(gl_InstanceID % 5 == 4) gl_Position = gl_Position + vec4(0.1, -0.1, 0.0, 0.0);
    }
}
