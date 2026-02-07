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

	if(i < FRACTAL_ITER) return float4(rgbColors(float2(x, y), cursorPos, i), 1.0);
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

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursor, i), 1.0);
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

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursorPos, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Power Set
float4 powerSet(float2 coord, float2 cursor){
	uint i = 0; // iteration count
	coord += float2(0.5, 0.5);
	cursor += float2(0.5, 0.5);

	// while(pow(pow(abs(coord.x + coord.y), abs(coord.x - coord.y)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	while(pow(pow(abs(coord.x / coord.y), abs(coord.y / coord.x)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
	// while(pow(pow(log(coord.x / coord.y), log(coord.y / coord.x)), (i + abs(coord.x * coord.y))) < FRACTAL_SIZE && i < FRACTAL_ITER){
		float x = abs(coord.x);
		float y = abs(coord.y);
		coord = float2(x + pow(x, y) * cursor.x, y + pow(y, x) * cursor.y);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursor, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Wing Set
float4 wingSet(float2 coord){
    uint i = 1;
	coord = float2(abs(coord.x + 0.5), abs(coord.y + 0.5));
    float x = coord.x; float y = coord.y;

    while(abs(x / coord.x) * abs(y / coord.y) < FRACTAL_SIZE && i < FRACTAL_ITER){
        x += smoothstep(coord.x - x, coord.x + x, i * i) * pow(coord.x / y, coord.x * i); // experiment with abs() for effects
        y -= smoothstep(coord.y - y, coord.y + y, sqrt(i)) * pow(coord.y / x, coord.y * i); // experiment with abs() for effects
        i++;
    }

    if (i < FRACTAL_ITER) return float4(rgbColors(float2(x, y), cursorPos, i), 1.0);
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

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursor, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}

// Loop Set
float4 loopSet(float2 coord){
	uint i = 1;

	while(length(coord) * atan(coord.y / coord.x) * i < FRACTAL_SIZE && i < FRACTAL_ITER){
		coord = float2(coord.x + cos(pow(i, i)), coord.y + sin(pow(i, i))) * atan(coord.x / coord.y);
		i++;
	}

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursorPos, i), 1.0);
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

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursor, i), 1.0);
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

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursorPos, i), 1.0);
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

	if (i < FRACTAL_ITER) return float4(rgbColors(coord, cursorPos, i), 1.0);
	return float4(0, 0, 0, 0); // black color within set
}