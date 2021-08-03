#version 440

layout(location = 0) flat in uvec2 screenRes;
layout(location = 1) flat in uvec2 cursorPos;

out vec4 color;

void main() {
	// Include effects here
	// vec2 fragCoordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // frag coords adjusted by window height and width
	vec2 fragCoordsAdj = vec2(gl_FragCoord.x / 1200, gl_FragCoord.y / 1100); // hard values

	uint multFactor = 10;
	color = vec4(sin(fragCoordsAdj.x * multFactor), sin(fragCoordsAdj.y * multFactor), 1.0, 1.0);
}
