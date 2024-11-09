#version 440

#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

layout(location = 0) out vec4 color_out;

// Main

void main() { color_out = vec4(1.0, 1.0, 0.0, 1.0); }
