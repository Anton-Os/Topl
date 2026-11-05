// Looping Patterns

float3 pattern1(float3 coords, float3 origin, uint iters){
    for(uint i = 0; i < iters; i++)
        for(int l = 0; l < 3; l++)
            if(dot(coords, origin) > 0) coords[l] *= -1;
    return coords - origin;
}

float3 pattern2(float3 coords, float3 origin, uint iters){
    for(uint i = 0; i < iters; i++)
        for(int l = 0; l < 3; l++)
            if(dot(coords, origin) < 0) coords[l] *= origin[l];
    return coords - origin;
}

float3 pattern3(float3 coords, float3 origin, uint iters){
    for(uint i = 0; i < iters; i++)
        for(int l = 0; l < 3; l++)
            if(length(coords) > length(origin)) coords *= origin / length(origin);
            else coords /= length(coords);
    return coords - origin;
}

// Hatched Patterns

float3 pattern4(float3 coords, float3 origin, uint xDiv, uint yDiv){
    float xStep = abs((coords.x - origin.x) * xDiv) - floor(abs((coords.x - origin.x) * xDiv));
    float yStep = abs((coords.y - origin.y) * yDiv) - floor(abs((coords.y - origin.y) * yDiv));
    return float3(xStep, yStep, coords.z - origin.z);
}

float3 pattern5(float3 coords, float3 origin, uint xDiv, uint yDiv){
    float xStep = abs((coords.x - origin.x) * xDiv) - floor(abs((coords.x - origin.x) * xDiv));
    float yStep = abs((coords.y - origin.y) * yDiv) - floor(abs((coords.y - origin.y) * yDiv));
    return cross(coords - origin, float3(xStep, yStep, coords.z - origin.z));
}

float3 pattern6(float3 coords, float3 origin, uint xDiv, uint yDiv){
    float xStep = abs((coords.x - origin.x) * xDiv) - floor(abs((coords.x - origin.x) * xDiv));
    float yStep = abs((coords.y - origin.y) * yDiv) - floor(abs((coords.y - origin.y) * yDiv));
    return float3(pow(coords.x - origin.x, xStep), pow(coords.y - origin.y, yStep), pow(coords.z - origin.z, dot(coords - origin, float3(xStep, yStep, coords.z - origin.z))));
}

// Timed Patterns

float3 pattern7(float3 coords, float3 origin, uint p, double t){
    float tStep = abs(sin(float(t)));
	return float3(
        sin((coords.x - origin.x) * p), 
        cos((coords.y - origin.y) * p), 
        tan((coords.z - origin.z) * p)
    ) * tStep;
}

float3 pattern8(float3 coords, float3 origin, uint p, double t){
    float tStep = abs(cos(float(t)));
    return float3(
        sin((coords.x - origin.x) * p) * tStep, 
        cos((coords.y - origin.y) * p) * tStep * 2.0, 
        tan((coords.z - origin.z) * p) * tStep * 3.0
    );
}

float3 pattern9(float3 coords, float3 origin, uint p, double t){
    float tStep = abs(tan(float(t)));
	return float3(
		sin((coords.x / origin.y) * p) / tStep, 
		cos((coords.y / origin.z) * p) / (tStep * 2.0),
		tan((coords.z / origin.x) * p) / (tStep * 3.0)
	);
}
