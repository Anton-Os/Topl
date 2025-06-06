#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.glsl"

#include "Pixel.glsl"

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	ivec2 screenRes; // resolution
	vec2 cursorPos;
	float effectSize;
	uint effectIters;
};

#define FRACTAL_SIZE effectSize
#define FRACTAL_ITER (effectIters * 5)

layout(location = 1) in vec3 texcoord;

layout(location = 0) out vec4 color_final;

// Functions

vec3 fractalColors(vec2 coord, vec2 cursor, uint i){
	float dist = pow(distance(coord, cursor), float(FRACTAL_SIZE - i));

	if(abs(mode) < 100){
		if(mode % 10 == 0) return vec3(1.0f / i, tan(i), 0.05f * i);
		else if(mode % 10 == 1) return vec3(getRandColor(i).r, getRandColor(i).g, getRandColor(i).b);
		else if(mode % 10 == 2) return vec3(pow(coord.x, i), pow(coord.y, 1.0 / i), pow(coord.x, coord.y));
		else if(mode % 10 == 3) return vec3(dist - floor(dist), ceil(dist) - dist, pow(dist, dist));
		else if(mode % 10 == 4) return vec3(coord.x * i - floor(coord.y * i), ceil(coord.y * i) - coord.x * i, cursor.x * cursor.y * i - floor(cursor.x * cursor.y * i));
		else if(mode % 10 == 5) return vec3(i / (FRACTAL_ITER * 0.5), (coord.x + coord.y) / FRACTAL_SIZE, ((coord.x - cursor.x) * (coord.y - cursor.y)) / FRACTAL_SIZE);
		else if(mode % 10 == 6) return vec3(dot(coord, cursor), length(coord - cursor), smoothstep(0.0, 1.0, pow(coord.x - cursor.x, coord.y - cursor.y))) * getRandColor(i);
		else if(mode % 10 == 7) return vec3(sin(1.0f / coord.x) + cos(1.0f / coord.y), atan(cursor.x / cursor.y), pow(abs(cursor.x + cursor.y), abs(coord.x * coord.y)));
		else if(mode % 10 == 8) return vec3(sin(dot(coord, cursor)), cos(dot(-coord, cursor)), tan(sin(coord.x - cursor.x) / cos(coord.y - cursor.y)));
		else if(mode % 10 == 9) return vec3((coord - cursor) * i, dot(coord, cursor) / i); // TODO: Change this!
		// else if(mode % 10 == 9) return getCoordDistances(coord, cursor, vec2(0.5f, 0.5f)) * vec3(mod(sinh((coord.x - cursor.x) * i), 1.0), mod(cosh((coord.y - cursor.y) * i), 1.0), mod(tanh((coord.x - cursor.x) * (coord.y - cursor.y) * i), 1.0));
		// else if(mode % 10 == 9) return vec3(abs(noise2(coord - cursor).x), abs(noise2(cursor - coord).y), noise1(float(i) * length(coord - cursor)));
		else return vec3(coord.x, coord.y, 1.0 / i);
	} else {
		float r = (float(i) / FRACTAL_ITER) * dist;
		float g = (float(i) / FRACTAL_ITER) * (coord.x / coord.y);
		float b = (float(i) / FRACTAL_ITER) * dot(coord, cursor);

		return vec3(r, g, b) * floor(abs(mode) / 100);
	}
	// TODO: Include more color options
}

// Mandlebrot Set

vec3 mandlebrot(vec2 coord, float f){
	uint i = 0; // iteration count
    float x = 0; float y = 0;

	while(pow(x, f) + pow(y, f) <= FRACTAL_SIZE && i < FRACTAL_ITER){
		float temp = pow(x, f) - pow(y, f) + coord.x;
		y = (2 * x * y) + coord.y;
		x = temp;
		i++;
	}

	if(i < FRACTAL_ITER) return fractalColors(vec2(x, y), cursorPos, i);
	else return vec3(0.0f, 0.0f, 0.0f); // black color within set
}


vec3 mandlebrotSet(vec2 coord){ return mandlebrot(coord, 2.0); }

// Julia set

vec3 julia(vec2 coord, vec2 cursor, float f){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		float x = pow(coord.x, f) - pow(coord.y, f);
		float y = 2.0 * coord.x * coord.y;
		coord = vec2(x, y) + cursor;
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 juliaSet(vec2 coord, vec2 cursor){ return julia(coord, cursor, 2.0); }

// Trig Set
vec3 trigSet(vec2 coord){
	uint i = 0; // iteration count

	while(abs(sin(coord.x) + cos(coord.y)) * abs(1.0 / (coord.x * coord.y)) < FRACTAL_SIZE && i < FRACTAL_ITER){
		double x = coord.x * sin(coord.y); // switch operations
		double y = coord.y * cos(coord.x); // switch operations
		coord = vec2(x, y);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

// Power set
vec3 powerSet(vec2 coord, vec2 cursor){
	uint i = 0; // iteration count

	// while(pow(pow(abs(coord.x + coord.y), abs(coord.x - coord.y)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	while(pow(pow(abs(coord.x / coord.y), abs(coord.y / coord.x)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	// while(pow(pow(log(coord.x / coord.y), log(coord.y / coord.x)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float x = abs(coord.x);
		float y = abs(coord.y);
		coord = vec2(x + pow(x, y) / cursor.x, y + pow(y, x) / cursor.y);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Wing Set
vec3 wingSet(vec2 coord){
    uint i = 1;
	coord = vec2(abs(coord.x), abs(coord.y));
    float x = coord.x; float y = coord.y;

    while(abs(x / coord.x) * abs(y / coord.y) < FRACTAL_SIZE && i < FRACTAL_ITER){
        // x += smoothstep(coord.x + x, coord.x - x, 1.0 / (i * i)) * pow(coord.x, x / y); // sin(y * i);
        // y += smoothstep(coord.y - y, coord.y + y, 1.0 / sqrt(i)) * pow(coord.y, y / x); // cos(x * i);
        x *= smoothstep(coord.x, coord.x + abs(x), i * i) * pow(coord.x / abs(y), coord.x * i); // experiment with abs() for effects
        y *= smoothstep(coord.y, coord.y + abs(y), sqrt(i)) * pow(coord.y / abs(x), coord.y * i); // experiment with abs() for effects
        i++;
    }

    if (i < FRACTAL_ITER) return fractalColors(vec2(x, y), cursorPos, i);
    return vec3(0, 0, 0);
}

// Step Set
vec3 stepSet(vec2 coord, vec2 cursor){
	uint i = 1; // iteration count

	while(((coord.y * (1.0 / coord.x)) - floor(coord.y * (1.0 / coord.x))) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		// coord = vec2((coord.x * i) - floor(coord.x * i), ceil(coord.y * i) - (coord.y * i));
		coord *= pow(i, dot(coord, cursor)) / pow(distance(coord, cursor), i);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Loop Set
vec3 loopSet(vec2 coord){
	uint i = 1;

	while(length(coord) * atan(coord.y / coord.x) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		coord = vec2(coord.x + cos(i * i), coord.y + sin(i * i)) * atan(coord.y / coord.x);
		// coord = vec2(coord.x + cos(i * length(coord)), coord.y + cos(i * length(coord)));
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

// Shard Set
vec3 shardSet(vec2 coord, vec2 cursor){
	uint i = 1; // iteration count

	while(length(coord - cursor) * (abs(coord.x - cursor.x) + abs(coord.y - cursor.y)) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		if(abs(coord.x - cursor.x) > abs(coord.y - cursor.y) * (1.0 / i)) coord.y *= 1.0 + length(coord - cursor);
		else if(abs(coord.y - cursor.y) > abs(coord.x - cursor.x) * (1.0 / i)) coord.x *= 1.0 + length(coord - cursor);
		// else coord += length(coord - cursor) / i;
		if(abs(coord.x - cursor.y) > abs(coord.y - cursor.x) * (1.0 / i)) coord.x += coord.y;
		else if(abs(coord.y - cursor.x) > abs(coord.x - cursor.y) * (1.0 / i)) coord.y += coord.x;
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Sparse Set
vec3 sparseSet(vec2 coord, float startMod){
	uint i = 1;
	float m = startMod;

	while((mod(coord.x, m) + mod(coord.y, m)) * (i / m) < FRACTAL_SIZE && i < FRACTAL_ITER){
		m *= 1.0 + abs(coord.x - coord.y);
		if(mod(m, startMod) > startMod / 2) coord += vec2(coord.x * m, coord.y / m);
		else coord -= vec2(coord.x / m, coord.y * m);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

// Retro Set
vec3 retroSet(vec2 coord, vec2 cursor){
	uint i = 1;
	vec2 points[5] = { vec2(0.0, 0.0), vec2(1.0, -1.0), vec2(1.0, 1.0), vec2(-0.1, 1.0), vec2(-1.0, -1.0) };
	vec2 nearestPoint = points[uint(floor(dot(coord, cursor) * 100)) % 5];

	while(distance(nearestPoint, points[(i - 1) % 5]) * abs(coord.x + coord.y) < FRACTAL_SIZE && i < FRACTAL_ITER){
		if(distance(points[(i - 1) % 5], coord - cursor) < distance(nearestPoint, coord - cursor)){
			coord -= nearestPoint / dot(coord - cursor, points[(i - 1) % 5]);
			nearestPoint = points[(i - 1) % 5];
		} else coord += points[(i - 1) % 5] * distance(coord - cursor, nearestPoint); // (1.0 / i);
		i++;
	}

	if (i < FRACTAL_ITER) return fractalColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}


// Recursive Fracals

vec3 recursiveAlgo(vec3 input1, vec3 input2, vec3 input3){
    uint i = 1;

    while(length(input1 - input2) * dot(input1, input3) < length(input2 - input3) * dot(input1, input2) && i < FRACTAL_ITER){
        input1 -= input3;
        input2 += input3;
        input3 *= dot(input1, input2);
        i++;
    }

    if(i >= FRACTAL_ITER || (input1.r == 0 && input1.g == 0 && input1.b == 0)) return vec3(0, 0, 0);
    else return fractalColors(vec2(input1.r - input3.b, input1.g + input3.r), vec2(input2.g - input3.r, input2.b + input3.g), i);
}

// Main

void main() {
	uint m = abs(mode) % 100;
	vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates
	float size = cam_pos.w * FRACTAL_SIZE;

	vec2 target;
	if(mode >= 0) target = coords - cursor - vec2(cam_pos.x, cam_pos.y);
	else target = vec2(texcoord.x, texcoord.y) - cursor;

	if(m >= 10 && m < 20) color_final = vec4(juliaSet(target * size, cursor), 1.0f);
	else if(m >= 20 && m < 30) color_final = vec4(trigSet((target + vec2(0.5, 0.5)) * size), 1.0f);
	else if(m >= 30 && m < 40) color_final = vec4(powerSet(target * size, cursorPos), 1.0f);
	else if(m >= 40 && m < 50) color_final = vec4(wingSet(target), 1.0f);
	else if(m >= 50 && m < 60) color_final = vec4(stepSet(target, cursorPos), 1.0f);
	else if(m >= 60 && m < 70) color_final = vec4(loopSet(target), 1.0F);
	else if(m >= 70 && m < 80) color_final = vec4(shardSet(target, cursorPos), 1.0f);
	else if(m >= 80 && m < 90) color_final = vec4(sparseSet(target, abs((target.x - cursor.x) - (target.y - cursor.y))), 1.0f);
	else if(m >= 90 && m < 100) color_final = vec4(retroSet(target, cursor), 1.0f);
	// else if(m >= 100) color_final = vec4(recursiveAlgo(trigSet(target), wingSet(target), mandlebrotSet(target)), 1.0);
	// else if(m >= 100) color_final = vec4(julia(target * size, cursor, 3.0)/*1.0 + (float(m) / 100.0f)) */, 1.0);
	else color_final = vec4(mandlebrotSet(target * size), 1.0f); // fractal mode

	if(color_final.r == 0.0f && color_final.g == 0.0f && color_final.b == 0.0f) color_final.a = 0.0; // make transparent if not in set
}
