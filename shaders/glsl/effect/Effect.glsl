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


vec3 mandlebrotSet(vec2 coord){ return mandlebrot(coord, 3.0); }

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

vec3 juliaSet(vec2 coord, vec2 cursor){ return julia(coord, cursor, 3.0); }

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

vec3 recursiveAlgo(vec3 target1, vec3 target2, vec3 target3){
    uint i = 1;

    while(length(target1 - target2) * dot(target1, target3) < length(target2 - target3) * dot(target1, target2) && i < FRACTAL_ITER){
        target1 -= target3;
        target2 += target3;
        target3 *= dot(target1, target2);
        i++;
    }

    if(i >= FRACTAL_ITER || (target1.r == 0 && target1.g == 0 && target1.b == 0)) return vec3(0, 0, 0);
    else return fractalColors(vec2(target1.r - target3.b, target1.g + target3.r), vec2(target2.g - target3.r, target2.b + target3.g), i);
}