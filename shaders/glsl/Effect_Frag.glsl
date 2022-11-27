#version 440

// Values

layout(std140, binding = 0) uniform Block{
	uint renderID;
};

layout(std140, binding = 1) uniform SceneBlock{
	ivec2 screenRes;
	vec2 cursorPos;
	uint mode;
};

out vec4 color;

// Functions

vec3 cursorDist(vec2 cursorPos, vec2 pixelCoord){
	float red = 1.0f - (distance(cursorPos, pixelCoord) * 5); // receding color from center
	float green = (distance(cursorPos, pixelCoord) < 0.03) ? 1.0 : 0.0; // small acute bullseye center
	float blue = (distance(cursorPos, pixelCoord) - 0.1) / 3; // increasing color from center
	return vec3(red, green, blue);
}

vec3 mandlebrot(uvec2 screenRes, vec2 pixelCoord){
	const double size = 3.0f;
	const uint max_iters = 1000;
	
	double x = 0; double y = 0;
	uint iter = 0;
	while(x * x + y * y <= size && iter < max_iters){
		double temp = (x * x) - (y * y) + pixelCoord.x;
		y = (2 * x * y) + pixelCoord.y;
		x = temp;
		iter++;
	}

	// Custom Colors
	if(iter < max_iters) return vec3(max(0.03f * iter, 0.5f), 0.0f, 1.0f / iter);
	else return vec3(0.0f, 0.0f, 0.0f);
}

// Main

void main() {
	vec2 cursorPosAdj = (cursorPos * 0.5f) + 0.5f;
	vec2 pixelCoordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // hard values

	if (mode == 1) color = vec4(mandlebrot(screenRes, pixelCoordsAdj + vec2(-0.25f, 0.5f)), 1.0f); // special mode
	else color = vec4(cursorDist(cursorPosAdj, pixelCoordsAdj), 1.0f); // cursor mode // default
}
