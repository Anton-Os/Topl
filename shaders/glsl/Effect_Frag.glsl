#version 440

layout(location = 0) flat in ivec2 screenRes;
layout(location = 1) flat in vec2 cursorPos;
layout(location = 2) flat in uint actorID;

out vec4 color;

vec3 mandlebrot(vec2 pixelCoord){
	// generate set
}

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

void main() {
	// Include effects here
	vec2 pixelCoordsAdj = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // hard values

	color = vec4(cursorDist(cursorPos, pixelCoordsAdj), 1.0);
	// color = vec4(colorQuad(pixelCoordsAdj), 1.0);
}
