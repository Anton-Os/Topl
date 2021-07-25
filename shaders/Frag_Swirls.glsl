#version 440

out vec4 color;

void main() {
	vec2 fragCoords = vec2(gl_FragCoord.x, gl_FragCoord.y);
	color = vec4(fragCoords.x, fragCoords.y, 0.0f, 1.0f);
}
