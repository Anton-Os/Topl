// Looping Patterns

vec3 loop_pattern1(vec3 coords, vec3 origin, uint iters){
	for(int l = 0; l < iters; l++)
        if(dot(coords, origin) > 0) coords[l] *= -1;
    return coords - origin;
}

vec3 loop_pattern2(vec3 coords, vec3 origin, uint iters){
	for(int l = 0; l < iters; l++)
        if(dot(coords, origin) < 0) coords[l] *= origin[l];
    return coords - origin;
}

vec3 loop_pattern3(vec3 coords, vec3 origin, uint iters){
	for(int l = 0; l < iters; l++)
        if(length(coords) > length(origin)) coords *= origin / length(origin);
        else coords /= length(coords);
    return coords - origin;
}

// Hatched Patterns

vec3 hatch_pattern1(vec3 coords, vec3 origin, uint xDiv, uint yDiv){
	float xStep = abs((coords.x - origin.x) * xDiv) - floor(abs((coords.x - origin.x) * xDiv));
	float yStep = abs((coords.y - origin.y) * yDiv) - floor(abs((coords.y - origin.y) * yDiv));
	return vec3(xStep, yStep, coords.z - origin.z);
}

vec3 hatch_pattern2(vec3 coords, vec3 origin, uint xDiv, uint yDiv){
	float xStep = abs((coords.x - origin.x) * xDiv) - floor(abs((coords.x - origin.x) * xDiv));
    float yStep = abs((coords.y - origin.y) * yDiv) - floor(abs((coords.y - origin.y) * yDiv));
    return cross(coords - origin, vec3(xStep, yStep, coords.z - origin.z));
}

vec3 hatch_pattern3(vec3 coords, vec3 origin, uint xDiv, uint yDiv){
	float xStep = abs((coords.x - origin.x) * xDiv) - floor(abs((coords.x - origin.x) * xDiv));
    float yStep = abs((coords.y - origin.y) * yDiv) - floor(abs((coords.y - origin.y) * yDiv));
    return vec3(pow(coords.x - origin.x, xStep), pow(coords.y - origin.y, yStep), pow(coords.z - origin.z, dot(coords - origin, vec3(xStep, yStep, coords.z - origin.z))));
}

// Timed Patterns

vec3 timed_pattern1(vec3 coords, vec3 origin, uint p, double t){
	float tStep = abs(sin(float(t)));
	return vec3(sin((coords.x - origin.x) * p), cos((coords.y - origin.y) * p), tan((coords.z - origin.z) * p)) * tStep;
}

vec3 timed_pattern2(vec3 coords, vec3 origin, uint p, double t){
	float tStep = abs(cos(float(t)));
	return vec3(
		sin((coords.x * origin.y) * p) * tStep, 
		cos((coords.y * origin.z) * p) * tStep * 2, 
		tan((coords.z * origin.x) * p) * tStep * 3
	);
}

vec3 timed_pattern3(vec3 coords, vec3 origin, uint p, double t){
	float tStep = abs(tan(float(t)));
	return vec3(
		sin((coords.x / origin.y) * p) / tStep, 
		cos((coords.y / origin.z) * p) / (tStep * 2), 
		tan((coords.z / origin.x) * p) / (tStep * 3)
	);
}
