// Color functions

vec3 rgbColors(vec2 coord, vec2 cursor, uint i){
	float dist = pow(distance(coord, cursor), float(FRACTAL_SIZE - i));

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
	// TODO: Include more color options
}

// Fractal functions

vec3 mandlebrot(vec2 coord, float f){ // Mandlebrot function
	uint i = 0; // iteration count
    float x = 0; float y = 0;

	while(pow(x, f) + pow(y, f) <= FRACTAL_SIZE && i < FRACTAL_ITER){
		float temp = pow(x, f) - pow(y, f) + coord.x;
		y = (2 * x * y) + coord.y;
		x = temp;
		i++;
	}

	if(i < FRACTAL_ITER) return rgbColors(vec2(x, y), cursorPos, i);
	else return vec3(0.0f, 0.0f, 0.0f); // black color within set
}

vec3 mandlebrotSet(vec2 coord){ return mandlebrot(coord, 2.0); }

vec3 julia(vec2 coord, vec2 cursor, float f){ // Julia function
	uint i = 0; // iteration count

	while (dot(coord, coord) <= FRACTAL_SIZE && i < FRACTAL_ITER) {
		float x = pow(coord.x, f) - pow(coord.y, f);
		float y = 2.0 * coord.x * coord.y;
		coord = vec2(x, y) + cursor;
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 juliaSet(vec2 coord, vec2 cursor){ return julia(coord, cursor, 2.0); }

vec3 trigSet(vec2 coord){ // Trig Set
	uint i = 0; // iteration count

	while(abs(sin(coord.x) + cos(coord.y)) * abs(1.0 / (coord.x * coord.y)) < FRACTAL_SIZE && i < FRACTAL_ITER){
		double x = coord.x * sin(coord.y); // switch operations
		double y = coord.y * cos(coord.x); // switch operations
		coord = vec2(x, y);
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 powerSet(vec2 coord, vec2 cursor){ // Power set
	uint i = 0; // iteration count

	// while(pow(pow(abs(coord.x + coord.y), abs(coord.x - coord.y)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	while(pow(pow(abs(coord.x / coord.y), abs(coord.y / coord.x)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	// while(pow(pow(log(coord.x / coord.y), log(coord.y / coord.x)), (float(i) + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float x = abs(coord.x);
		float y = abs(coord.y);
		coord = vec2(x + pow(x, y) / cursor.x, y + pow(y, x) / cursor.y);
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 wingSet(vec2 coord){ // Wing Set
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

    if (i < FRACTAL_ITER) return rgbColors(vec2(x, y), cursorPos, i);
    return vec3(0, 0, 0);
}

vec3 stepSet(vec2 coord, vec2 cursor){ // Step Set
	uint i = 1; // iteration count

	while(((coord.y * (1.0 / coord.x)) - floor(coord.y * (1.0 / coord.x))) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		// coord = vec2((coord.x * i) - floor(coord.x * i), ceil(coord.y * i) - (coord.y * i));
		coord *= pow(i, dot(coord, cursor)) / pow(distance(coord, cursor), i);
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 loopSet(vec2 coord){ // Loop Set
	uint i = 1;

	while(length(coord) * atan(coord.y / coord.x) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		coord = vec2(coord.x + cos(i * i), coord.y + sin(i * i)) * atan(coord.y / coord.x);
		// coord = vec2(coord.x + cos(i * length(coord)), coord.y + cos(i * length(coord)));
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 shardSet(vec2 coord, vec2 cursor){ // Shard Set
	uint i = 1; // iteration count

	while(length(coord - cursor) * (abs(coord.x - cursor.x) + abs(coord.y - cursor.y)) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		if(abs(coord.x - cursor.x) > abs(coord.y - cursor.y) * (1.0 / i)) coord.y *= 1.0 + length(coord - cursor);
		else if(abs(coord.y - cursor.y) > abs(coord.x - cursor.x) * (1.0 / i)) coord.x *= 1.0 + length(coord - cursor);
		// else coord += length(coord - cursor) / i;
		if(abs(coord.x - cursor.y) > abs(coord.y - cursor.x) * (1.0 / i)) coord.x += coord.y;
		else if(abs(coord.y - cursor.x) > abs(coord.x - cursor.y) * (1.0 / i)) coord.y += coord.x;
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 sparseSet(vec2 coord, float startMod){ // Sparse Set
	uint i = 1;
	float m = startMod;

	while((mod(coord.x, m) + mod(coord.y, m)) * (i / m) < FRACTAL_SIZE && i < FRACTAL_ITER){
		m *= 1.0 + abs(coord.x - coord.y);
		if(mod(m, startMod) > startMod / 2) coord += vec2(coord.x * m, coord.y / m);
		else coord -= vec2(coord.x / m, coord.y * m);
		i++;
	}

	if (i < FRACTAL_ITER) return rgbColors(coord, cursorPos, i);
	return vec3(0, 0, 0); // black color within set
}

vec3 retroSet(vec2 coord, vec2 cursor){ // Retro Set
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

	if (i < FRACTAL_ITER) return rgbColors(coord, cursor, i);
	return vec3(0, 0, 0); // black color within set
}

// Recursive functions

vec3 recursiveAlgo(vec3 target1, vec3 target2, vec3 target3){
    uint i = 1;

    while(length(target1 - target2) * dot(target1, target3) < length(target2 - target3) * dot(target1, target2) && i < FRACTAL_ITER){
        target1 -= target3;
        target2 += target3;
        target3 *= dot(target1, target2);
        i++;
    }

    if(i >= FRACTAL_ITER || (target1.r == 0 && target1.g == 0 && target1.b == 0)) return vec3(0, 0, 0);
    else return rgbColors(vec2(target1.r - target3.b, target1.g + target3.r), vec2(target2.g - target3.r, target2.b + target3.g), i);
}
