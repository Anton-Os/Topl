#version 440

layout(packed) uniform Block{
	vec2 rotation; // padding from vec2 to vec4
	vec3 offset; // padding from vec3 to vec4
    uint flatColor;
};

layout(location = 0) in vec3 pos;

layout(location = 0) out uint flatColor_out;

void main() {
	vec3 finalPos = pos;
	if (rotation.x != 0) {
		mat2 rotMatrix = mat2(
			cos(rotation.x), sin(rotation.x),
			-1 * sin(rotation.x), cos(rotation.x)
		);

		vec2 rotCoords = rotMatrix * vec2(finalPos.x, finalPos.y);
		finalPos.x = rotCoords.x;
		finalPos.y = rotCoords.y;
	}

    flatColor_out = 0xFF884422; // Make this equal to flatColor input
	gl_Position = vec4(finalPos + vec3(offset), 1.0);
}