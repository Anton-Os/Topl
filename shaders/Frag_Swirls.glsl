#version 440

#define WIN_HEIGHT 1200
#define WIN_WIDTH 1100

out vec4 color;

void main() {
	vec2 fragCoordsAdj = vec2(gl_FragCoord.x / WIN_HEIGHT, gl_FragCoord.y / WIN_WIDTH); // frag coords adjusted by window height and width
	// color = vec4(fragCoords.x / WIN_WIDTH, fragCoords.y / WIN_HEIGHT, 0.0f, 1.0f);
	int multFactor = 10;
	color = vec4(sin(fragCoordsAdj.x * multFactor), sin(fragCoordsAdj.y * multFactor), 1.0, 1.0);
}
