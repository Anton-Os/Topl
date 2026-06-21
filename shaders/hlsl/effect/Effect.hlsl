float3 rgbColors(float2 coord, float2 cursor, uint i){
	float dist = distance(coord, cursor) * float(1.0 / i);

	if(abs(mode) % 10 == 0) return float3(1.0f / i, tan(i), 0.05f * i);
	else if(abs(mode) % 10 == 1) return float3(getRandColor(i).r, getRandColor(i).g, getRandColor(i).b);
	else if(abs(mode) % 10 == 2) return float3(pow(coord.x, i), pow(coord.y, 1.0 / i), pow(coord.x, coord.y));
	else if(abs(mode) % 10 == 3) return float3(dist - floor(dist), ceil(dist) - dist, pow(dist, dist));
	else if(abs(mode) % 10 == 4) return float3(coord.x * i - floor(coord.x * i), ceil(coord.y * i) - coord.y * i, abs(dot(cursor, coord)) - floor(abs(dot(cursor, coord))));
	else if(abs(mode) % 10 == 5) return float3(i / (FRACTAL_ITER * 0.75), (coord.x + coord.y) / FRACTAL_SIZE, ((coord.x - cursor.x) * (coord.y - cursor.y)) / FRACTAL_SIZE);
	else if(abs(mode) % 10 == 6) return getRandColor(i) * float3(length(coord - cursor), dot(coord, cursor), smoothstep(0.0, 1.0, pow(coord.x - cursor.x, coord.y - cursor.y)));
	else if(abs(mode) % 10 == 7) return float3(sin(1.0f / coord.y) + cos(1.0f / coord.x), atan(cursor.y / cursor.x), pow(abs(cursor.x + coord.y), abs(coord.x * cursor.y)));
	else if(abs(mode) % 10 == 8) return float3(sin(dot(coord, cursor)), sin(dot(-coord, cursor)), tan(cos(coord.x - cursor.x) / sin(coord.y - cursor.y)));
	else if(abs(mode) % 10 == 9) return float3((coord - cursor) * i, dot(coord, cursor) / i); // TODO: Change this!
	// getCoordDistances(coord, cursor, float2(0.5f, 0.5f)) * float3(pow(coord.x - cursor.x, 1.0 / i), pow(i, coord.y - cursor.y), pow(coord.x - cursor.x, coord.y - cursor.y));
	else return float3(coord.x, coord.y, 1.0 / i);
}

float3 dynamicColors(float2 coord, float2 cursor, float f, uint i){
	float dist = distance(coord, cursor) * float(1.0 / i);

	float r = (coord.x - cursor.x) * (f / i); 
	float g = (coord.y - cursor.y) * (f / i); 
	float b = dist * float(i);

	return float3(r, g, b) * ((abs(mode) % 10) + 1); // TODO: Modify this
}