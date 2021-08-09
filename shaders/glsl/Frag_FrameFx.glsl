#version 440

layout(location = 0) flat in ivec2 screenRes;
layout(location = 1) flat in vec2 cursorPos;
layout(location = 2) flat in uint renderID;

out vec4 color;

void main() {
	// Include effects here
	vec2 fragCoordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // hard values

	uint m = 10;
	if(cursorPos.x > 0.0)
		color = vec4(sin(fragCoordsAdj.x * m), sin(fragCoordsAdj.y * m), 1.0, 1.0);
	else 
		color = vec4(cos(fragCoordsAdj.x * m), cos(fragCoordsAdj.y * m), 1.0, 1.0);
}
