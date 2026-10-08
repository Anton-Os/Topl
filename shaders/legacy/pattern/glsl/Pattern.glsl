vec3 pattern1(vec3 coords, vec3 origin){
	return coords - origin;
}

vec3 pattern2(vec3 coords, vec3 origin){
	return coords + origin;
}

vec3 pattern3(vec3 coords, vec3 origin){
	return coords * origin;
}

vec3 pattern4(vec3 coords, vec3 origin){
	return coords / origin;
}

vec3 pattern5(vec3 coords, vec3 origin){
	return cross(coords, origin);
}

vec3 pattern6(vec3 coords, vec3 origin){
	return vec3(pow(coords.x, origin.x), pow(coords.y, origin.y), pow(coords.z, origin.z));
}

vec3 pattern7(vec3 coords, vec3 origin){
	return vec3(pow(origin.x, coords.x), pow(origin.y, coords.y), pow(origin.z, coords.z));
}

vec3 pattern8(vec3 coords, vec3 origin){
	return vec3(sin(coords.x * origin.x), cos(coords.y * origin.y), tan(coords.z * origin.z));
}

vec3 pattern9(vec3 coords, vec3 origin){
	return vec3(sin(coords.x / origin.y), cos(coords.y / origin.z), tan(coords.z / origin.x));
}
