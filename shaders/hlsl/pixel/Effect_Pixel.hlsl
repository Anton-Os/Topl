#define INCLUDE_BLOCK
#define IGNORE_INPUTS

#include "Common.hlsl"

#include "Pixel.hlsl"

// Values

cbuffer CONST_SCENE_BLOCK : register(b1) {
	int mode;
	float4 cam_pos;
	float4 look_pos;
	float4x4 projMatrix;

	int2 screenRes;
	float2 cursorPos;
	float effectSize;
	uint effectIters;
}

#define FRACTAL_SIZE 10.0
#define FRACTAL_ITER 15

struct PS_INPUT { 
	float4 pos : SV_POSITION;
	float3 texcoord : TEXCOORD0;
};

// Functions

float3 fractalColors(float2 coord, float2 cursor, uint i){
	if(abs(mode) % 10 == 0) return float3(1.0f / i, tan(i), 0.05f * i);
	else if(abs(mode) % 10 == 1) return float3(getRandColor(i).r, getRandColor(i).g, getRandColor(i).b);
	else if(abs(mode) % 10 == 2) return float3(pow(coord.x, i), pow(coord.y, 1.0 / i), pow(coord.x, coord.y));
	else if(abs(mode) % 10 == 3) return float3(distance(coord - cursor, float2(0.0, 0.0)), distance(cursor, float2(0.0, 0.0)), distance(coord, cursor));
	else if(abs(mode) % 10 == 4) return float3(coord.x * i - floor(coord.x * i), ceil(coord.y * i) - coord.y * i, abs(dot(cursor, coord)) - floor(abs(dot(cursor, coord))));
    else if(abs(mode) % 10 == 5) return float3(i / (FRACTAL_ITER * 0.75), (coord.x + coord.y) / FRACTAL_SIZE, ((coord.x - cursor.x) * (coord.y - cursor.y)) / FRACTAL_SIZE);
    else if(abs(mode) % 10 == 6) return getRandColor(i) * float3(length(coord - cursor), dot(coord, cursor), smoothstep(0.0, 1.0, pow(coord.x - cursor.x, coord.y - cursor.y)));
    else if(abs(mode) % 10 == 7) return float3(sin(1.0f / coord.y) + cos(1.0f / coord.x), atan(cursor.y / cursor.x), pow(abs(cursor.x + coord.y), abs(coord.x * cursor.y)));
    else if(abs(mode) % 10 == 8) return float3(sin(dot(coord, cursor)), sin(dot(-coord, cursor)), tan(cos(coord.x - cursor.x) / sin(coord.y - cursor.y)));
    else if(abs(mode) % 10 == 9) return float3((coord - cursor) * i, dot(coord, cursor) / i); // TODO: Change this!
	// getCoordDistances(coord, cursor, float2(0.5f, 0.5f)) * float3(pow(coord.x - cursor.x, 1.0 / i), pow(i, coord.y - cursor.y), pow(coord.x - cursor.x, coord.y - cursor.y));
    else return float3(coord.x, coord.y, 1.0 / i);
	// TODO: Include more color options
}

// Mandlebrot Set
float4 mandlebrotSet(float2 coord){
	uint i = 0; // iteration count
	double x = 0; double y = 0;

	while(x * x + y * y <= FRACTAL_SIZE && i < FRACTAL_ITER){
		double temp = (x * x) - (y * y) + coord.x;
		y = (2 * x * y) + coord.y;
		x = temp;
		i++;
	}

	if(i < FRACTAL_ITER) return float4(fractalColors(float2(x, y), cursorPos, i), 1.0);
	else return float4(0.0f, 0.0f, 0.0f, 0.0f); // black color within set
}

// Julia set
float4 juliaSet(float2 coord, float2 cursor){
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = (coord.x * coord.x) - (coord.y * coord.y);
		double y = 2.0 * coord.x * coord.y;
		coord = float2(x, y) + cursor;
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursor, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Trig set
float4 trigSet(float2 coord){
	uint i = 0; // iteration count

	/* while (atan(coord.x) + tan(coord.y) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		double x = coord.x + sin(coord.y / coord.x);
		double y = coord.y + cos(coord.x / coord.y);
		coord = float2(x, y);
		i++;
	} */

	while(abs(sin(coord.x) + cos(coord.y)) * abs(1.0 / (coord.x * coord.y)) < FRACTAL_SIZE && i < FRACTAL_ITER){
		double x = coord.x + sin(coord.y);
		double y = coord.y + cos(coord.x);
		coord = float2(x, y);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursorPos, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Power Set
float4 powerSet(float2 coord, float2 cursor){
	uint i = 0; // iteration count

	// while(pow(pow(abs(coord.x + coord.y), abs(coord.x - coord.y)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	while(pow(pow(abs(coord.x / coord.y), abs(coord.y / coord.x)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	// while(pow(pow(log(coord.x / coord.y), log(coord.y / coord.x)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float x = abs(coord.x);
		float y = abs(coord.y);
		coord = float2(x + pow(x, y) * cursor.x, y + pow(y, x) * cursor.y);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursor, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Wing Set
float4 wingSet(float2 coord){
    uint i = 1;
	coord = float2(abs(coord.x), abs(coord.y));
    float x = coord.x; float y = coord.y;

    while(abs(x / coord.x) * abs(y / coord.y) < FRACTAL_SIZE && i < FRACTAL_ITER){
        x += smoothstep(coord.x - x, coord.x + x, i * i) * pow(coord.x / y, coord.x * i); // experiment with abs() for effects
        y -= smoothstep(coord.y - y, coord.y + y, sqrt(i)) * pow(coord.y / x, coord.y * i); // experiment with abs() for effects
        i++;
    }

    if (i < FRACTAL_ITER) return float4(fractalColors(float2(x, y), cursorPos, i), 1.0);
    return float4(0, 0, 0, 0);
}

// Step Set
float4 stepSet(float2 coord, float2 cursor){
	uint i = 1; // iteration count

	while(((coord.y * (1.0 / coord.x)) - floor(coord.y * (1.0 / coord.x))) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		// coord = float2((coord.x * i) - floor(coord.x * i), ceil(coord.y * i) - (coord.y * i));
		coord *= i * distance(coord, cursor);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursor, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Loop Set
float4 loopSet(float2 coord){
	uint i = 1;

	while(length(coord) * atan(coord.y / coord.x) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		coord = float2(coord.x + cos(pow(i, i)), coord.y + sin(pow(i, i))) * atan(coord.x / coord.y);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursorPos, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Shard Set
float4 shardSet(float2 coord, float2 cursor){
	uint i = 1; // iteration count

	while(distance(coord, cursor) * atan((coord.x - cursor.x) / (coord.y - cursor.y)) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float angle = atan((coord.x - cursor.x) / (coord.y - cursor.y));
		if(distance(coord, cursor) < angle) coord += distance(coord, cursor) * abs(dot(coord, cursor) / i);
		else coord *= distance(coord, cursor) * angle * i;
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursor, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Sparse Set
float4 sparseSet(float2 coord, float startMod){
	uint i = 1; // iteration count
	float m = startMod;

	while((fmod(coord.x, m) + fmod(coord.y, m)) * (i / m) < FRACTAL_SIZE && i < FRACTAL_ITER){
		m += abs(coord.x - coord.y);
		if(fmod(m, startMod) > startMod / 2) coord += float2(coord.x * m, coord.y / m);
		else coord -= float2(coord.x / m, coord.y * m);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursorPos, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Retro Set
float4 retroSet(float2 coord, float2 cursor){
	uint i = 1;
	float2 points[5] = { float2(0.0, 0.0), float2(1.0, -1.0), float2(1.0, 1.0), float2(-0.1, 1.0), float2(-1.0, -1.0) };
	float2 nearestPoint = points[0];

	for(uint n = 1; n < 5; n++) if(distance(points[n], coord) < distance(nearestPoint, coord)) nearestPoint = points[n];

	while(distance(nearestPoint, points[(i - 1) % 5]) * abs(coord.x + coord.y) < FRACTAL_SIZE && i < FRACTAL_ITER){
		if(distance(points[(i - 1) % 5], coord - cursor) < distance(nearestPoint, coord - cursor)){
			coord += nearestPoint * sin(float(i) * cursor.x); // (1.0 / i);
			nearestPoint = points[(i - 1) % 5];
		} else coord -= points[(i - 1) % 5] * cos(float(i) * cursor.y); // (1.0 / i);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(fractalColors(coord, cursorPos, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Main

float4 main(PS_INPUT input) : SV_TARGET{
	float2 cursor = ((cursorPos * float2(1.0f, -1.0f)) * 0.5f) + 0.5f; // adjusted cursor
	float2 coords = float2(input.pos.x / screenRes.x, input.pos.y / screenRes.y); // adjusted coordinates

	float2 target = coords - cursor;
	if(mode >= 0) target = coords - cursor; 
	else target = float2(input.texcoord.x, input.texcoord.y) - cursor;
	target += float2(cam_pos.x, cam_pos.y);
	target *= FRACTAL_SIZE;

    if(abs(mode) >= 10 && abs(mode) < 20) return juliaSet(target, cursor);
	else if(abs(mode) >= 20 && abs(mode) < 30) return trigSet(target);
	else if(abs(mode) >= 30 && abs(mode) < 40) return powerSet(target, cursorPos);
    else if(abs(mode) >= 40 && abs(mode) < 50) return wingSet(target);
    else if(abs(mode) >= 50 && abs(mode) < 60) return stepSet(target, cursorPos);
	else if(abs(mode) >= 60 && abs(mode) < 70) return loopSet(target);
	else if(abs(mode) >= 70 && abs(mode) < 80) return shardSet(target, cursorPos);
	else if(abs(mode) >= 80 && abs(mode) < 90) return sparseSet(target, distance(coords, cursor));
	else if(abs(mode) >= 90 && abs(mode) < 100) return retroSet(target, cursor);
	else if(abs(mode) >= 100 && abs(mode) < 110) return juliaSet(float2(mandlebrotSet(target).r, mandlebrotSet(target).g), cursor);
	else if(abs(mode) >= 110 && abs(mode) < 120) return trigSet(float2(tan(powerSet(target, cursor).r), 1.0 / tan(powerSet(target, cursor).g)));
	else if(abs(mode) >= 120 && abs(mode) < 130) return stepSet(float2(wingSet(target).r * wingSet(target).b, wingSet(target).g / wingSet(target).b), cursor);
    else return mandlebrotSet(target);
}
