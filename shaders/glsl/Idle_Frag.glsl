#version 440

#define INCLUDE_BLOCK
#define INCLUD_SCENEBLOCK
#define INCLUDE_EXTBLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

// Values

layout(location = 0) out vec4 color_out;

// Main

void main() { 
    if(vertCount == 4) color_out = vec4(0.0, 1.0, 1.0, 1.0);
    else if(vertCount == 0) color_out = vec4(1.0, 0.0, 1.0, 1.0);
    else color_out = vec4(1.0, 1.0, 0.0, 1.0); 
}
