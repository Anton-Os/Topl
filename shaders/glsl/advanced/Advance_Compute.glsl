#version 430 core

#define IGNORE_INPUTS
#define INCLUDE_DATASTREAM

#include "../Common.glsl"

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

/* layout(std430, binding = 0) buffer result {
    writeonly restrict float output;
} */
 
void main(){
    // output = 0.0;
}
