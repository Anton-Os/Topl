#version 440

#define INCLUDE_BLOCK
#define IGNORE_INPUTS
#define INCLUDE_TEXTURES

#include "Common.glsl"

#define CURSOR_SIZE 0.05
#define TRACER_STEPS 8
#define TRACER_PATHS 8

// Values

layout(std140, binding = 1) uniform SceneBlock {
	int mode;
	vec4 cam_pos;
	vec3 look_pos;
	mat4 projMatrix;
	
	ivec2 screenRes; // resolution
	vec2 cursorPos;
	vec2 tracerSteps[TRACER_STEPS];
	vec2 tracerPaths[TRACER_PATHS];
};

layout(location = 0) in vec3 texcoord;

layout(location = 0) out vec4 color_out;

// Cursor Functions

vec4 cursorDot(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) < radius) return color;
    else return vec4(color.r, color.g, color.b, 0.0);
}

vec4 cursorHalo(vec2 pos, vec2 coord, float radius, vec4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return vec4(color.r, color.g, color.b, 0.0);
}

vec4 cursorCross(vec2 pos, vec2 coord, float radius, vec4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1) || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return vec4(color.r, color.g, color.b, 0.0);
}

// Draw Functions

vec4 drawLines(vec2 pos, vec2 coord, float dist, vec4 color){
    uint t = 0;
    vec4 color_draw = vec4(color.r, color.g, color.b, 0.0);

    while(tracerSteps[t].x != 0.0 && tracerSteps[t].y != 0.0 && t < TRACER_STEPS){
        vec2 step1 = ((tracerSteps[t] * 0.5f) + 1.0) - coord;
        vec2 step2 = ((tracerSteps[t + 1] * 0.5f) + 1.0) - coord;

        float lineDist = getLineDistance(coord, step1, step2);
        vec3 distances = getCoordDistances(coord, step1, step2);
        if(lineDist < dist && distances[1] < distances[0] && distances[2] < distances[0]) color_draw = color;

        t++;
    }

    return color_draw;
}

vec4 drawCurves(vec2 pos, vec2 coord, float dist, vec4 color){
    uint t = 0;
    vec4 color_draw = vec4(color.r, color.g, color.b, 0.0);

    while(tracerSteps[t].x != 0.0 && tracerSteps[t].y != 0.0 && t < TRACER_STEPS){
        vec2 step1 = ((tracerSteps[t] * 0.5f) + 1.0) - coord;
        vec2 step2 = ((tracerSteps[t + 1] * 0.5f) + 1.0) - coord;

        float lineDist = getLineDistance(coord, step1, step2);
        vec3 distances = getCoordDistances(coord, step1, step2);
        if(sin(lineDist) < dist && distances[1] < distances[0] && distances[2] < distances[0]) color_draw = color;

        t++;
    }

    return color_draw;
}

vec4 drawZigZags(vec2 pos, vec2 coord, float dist, vec4 color){
    uint t = 0;
    vec4 color_draw = vec4(color.r, color.g, color.b, 0.0);

    while(tracerSteps[t].x != 0.0 && tracerSteps[t].y != 0.0 && t < TRACER_STEPS){
        vec2 step1 = ((tracerSteps[t] * 0.5f) + 1.0) - coord;
        vec2 step2 = ((tracerSteps[t + 1] * 0.5f) + 1.0) - coord;

        float lineDist = getLineDistance(coord, step1, step2);
        vec3 distances = getCoordDistances(coord, step1, step2);
        if((lineDist * 10) - floor(lineDist * 10) < dist && distances[1] < distances[0] && distances[2] < distances[0]) color_draw = color;

        t++;
    }

    return drawLines(pos, coord, dist, color); // color_draw;
}

// Main

void main() {
    if(mode < 0) color_out = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // base texture

	vec2 cursor = (cursorPos * 0.5f) + 0.5f; // adjusted cursor
	vec2 coords = vec2(gl_FragCoord.x / screenRes.x, gl_FragCoord.y / screenRes.y); // adjusted coordinates
	float size = CURSOR_SIZE * (floor(abs(mode) / 100.0) + 1);

	if(abs(mode) % 10 != 0){
        vec4 color_draw;
        if(mode > 0) color_draw = color_correct(texture(baseTex, vec2(texcoord.x, texcoord.y))); // draw
        else color_draw = vec4(0.0, 0.0, 0.0, 0.0); // erase

        if(abs(mode) % 10 == 1) color_draw = drawLines(cursor, coords, size, color_draw);
        else if(abs(mode % 10) == 2) color_draw = drawCurves(cursor, coords, size, color_draw);
        else if(abs(mode % 10) == 3) color_draw = drawZigZags(cursor, coords, size, color_draw);

        if(color_draw.a != 0.0 && mode >= 0) color_out = color_draw;
        // else if(color_draw.a == 0.0 && mode < 0) color_out = color_draw;
    }

	if(abs(mode) >= 10){
		vec4 color_cursor = vec4(0.0, 0.0, 0.0, 0.0);
		if(uint(floor(abs(mode) / 10.0)) % 10 == 1) color_cursor = cursorDot(cursor, coords, size, vec4(1.0, 1.0, 1.0, 0.75));
		else if(uint(floor(abs(mode) / 10.0)) % 10 == 2) color_cursor = cursorHalo(cursor, coords, size, vec4(1.0, 1.0, 1.0, 0.75));
		else color_cursor = cursorCross(cursor, coords, size, vec4(1.0, 1.0, 1.0, 0.75));

		if(color_cursor.a != 0.0) color_out = color_cursor;
	}
}
