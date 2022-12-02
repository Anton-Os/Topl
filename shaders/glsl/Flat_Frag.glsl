#version 440

// Values

layout(location = 0) in vec4 flatColor;

layout(location = 0) out vec4 color;

// Main

void main() { color = flatColor; }
