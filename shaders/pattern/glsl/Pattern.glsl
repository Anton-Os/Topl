/* vec3 tagPattern(int vertex_id, vec3 coords){
	if(vertex_id % 3 == 0) return vec3(sin(coords.x), cos(coords.y), tan(coords.z));
	else if(vertex_id % 3 == 1) return vec3(sin(-coords.x), cos(-coords.y), tan(-coords.z));
	else return coords * (1.0 / vertex_id);
} */

vec3 pattern1(vec3 coords, uint m){
	return coords * m;
}

vec3 pattern2(vec3 coords, uint m){
	return coords * m;
}

vec3 pattern3(vec3 coords, uint m){
	return coords * m;
}

vec3 pattern4(vec3 coords, uint m, float i){
	return coords * (m / i);
}

vec3 pattern5(vec3 coords, uint m, float i){
	return coords * (m / i);
}

vec3 pattern6(vec3 coords, uint m){
	return coords * m;
}

vec3 pattern7(vec3 coords, uint m){
	return coords * m;
}

vec3 pattern8(vec3 coords, uint m, float t){
	return coords * m * cos(t);
}

vec3 pattern9(vec3 coords, uint m, float t){
	return coords * m * cos(t);
}
