#version 440

layout(location = 0) flat in ivec2 screenRes;
layout(location = 1) flat in vec2 cursorPos;
layout(location = 2) flat in uint renderID;

out vec4 color;

vec3 mandlebrot(vec2 fragCoord){
	const float origin_len = 1.0f;
	const float max_steps = 1000.0f;
	float steps = 0.0f;

	// TODO: Add set generation logic

	float f = steps / max_steps;
	return vec3(f); // should return black color without any further calculations
}

vec3 cursorDist(vec2 cursorPos, vec2 fragCoord){
	float red = 1.0f - (distance(cursorPos, fragCoord) * 5); // receding color from center
	float green = (distance(cursorPos, fragCoord) < 0.03) ? 1.0 : 0.0; // small acute bullseye center
	float blue = (distance(cursorPos, fragCoord) - 0.1) / 5; // increasing color from center
	return vec3(red, green, blue);
}

vec3 colorQuad(vec2 fragCoord){
	if(fragCoord.x > 0.5 && fragCoord.y > 0.0) return vec3(1.0f, 0.0f, 0.0f);
	else if(fragCoord.x < 0.5 && fragCoord.y > 0.0) return vec3(0.0f, 1.0f, 0.0f);
	else if(fragCoord.x < 0.5 && fragCoord.y < 0.0) return vec3(0.0f, 0.0f, 1.0f);
	else return vec3(0.0f, 0.0f, 0.0f);
}

void main() {
	// Include effects here
	vec2 fragCoordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // hard values

	color = vec4(cursorDist(cursorPos, fragCoordsAdj), 1.0);
	// color = vec4(colorQuad(fragCoordsAdj), 1.0);
	// color = vec4(fragCoordsAdj.x, 0.0, fragCoordsAdj.y, 1.0);
}
