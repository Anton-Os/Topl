#version 440

layout(location = 0) flat in ivec2 screenRes;
layout(location = 1) flat in vec2 cursorPos;
layout(location = 2) flat in uint actorID;

out vec4 color;

vec3 cursorDist(vec2 cursorPos, vec2 pixelCoord){
	float red = 1.0f - (distance(cursorPos, pixelCoord) * 5); // receding color from center
	float green = (distance(cursorPos, pixelCoord) < 0.03) ? 1.0 : 0.0; // small acute bullseye center
	float blue = (distance(cursorPos, pixelCoord) - 0.1) / 5; // increasing color from center
	return vec3(red, green, blue);
}

vec3 colorQuad(vec2 pixelCoord){
	if(pixelCoord.x > 0.5 && pixelCoord.y > 0.0) return vec3(1.0f, 0.0f, 0.0f);
	else if(pixelCoord.x < 0.5 && pixelCoord.y > 0.0) return vec3(0.0f, 1.0f, 0.0f);
	else if(pixelCoord.x < 0.5 && pixelCoord.y < 0.0) return vec3(0.0f, 0.0f, 1.0f);
	else return vec3(0.0f, 0.0f, 0.0f);
}

vec3 mandlebrot(uvec2 screenRes, vec2 pixelCoord){
	const double size = 10.0f;
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

void main() {
	// Include effects here
	vec2 pixelCoordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // hard values
	vec2 pixelOffset = vec2(-0.25f, -0.25f);

	color = vec4(cursorDist(cursorPos, pixelCoordsAdj), 1.0);
	// color = vec4(colorQuad(pixelCoordsAdj), 1.0);
	// color = vec4(mandlebrot(screenRes, pixelCoordsAdj + pixelOffset), 1.0);
}
