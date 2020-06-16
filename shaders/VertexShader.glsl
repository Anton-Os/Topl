#version 440

in vec2 offset;

in vec3 vp;

void main(){
    gl_Position = vec4(vp, 1.0);
}