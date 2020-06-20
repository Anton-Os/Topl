#version 440

layout(std140) uniform Block {
    vec3 offset;
};

// uniform float offset[2];

in vec3 vp;

void main(){
    gl_Position = vec4(vp + offset, 1.0);
}