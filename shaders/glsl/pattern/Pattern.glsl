/* vec3 tagPattern(int vertex_id, vec3 coords){
	if(vertex_id % 3 == 0) return vec3(sin(coords.x), cos(coords.y), tan(coords.z));
	else if(vertex_id % 3 == 1) return vec3(sin(-coords.x), cos(-coords.y), tan(-coords.z));
	else return coords * (1.0 / vertex_id);
} */

vec3 pattern1(vec3 coords, uint m){
	float r = ((coords.x * 5) - floor(coords.x * 5)) * m; // pow(coords.x, coords.y) * m;
	float g = ((coords.y * 10) - floor(coords.y * 10)) * m; // pow(coords.y, coords.z) * m;
	float b = ((coords.z * 50) - floor(coords.z * 50)) * m; // pow(coords.z, coords.x) * m;

	return vec3(abs(r) - floor(abs(r)), abs(g) - floor(abs(g)), abs(b) - floor(abs(b)));
}

vec3 pattern2(vec3 coords, uint m){
	float r = pow(abs(coords.x + coords.z), abs(coords.y)) * m;
	float g = pow(abs(coords.z - coords.y), abs(coords.x)) * m;
	float b = pow(abs(coords.y * coords.x), abs(coords.z)) * m;

	return vec3(abs(r) - floor(abs(r)), abs(g) - floor(abs(g)), abs(b) - floor(abs(b)));
}

vec3 pattern3(vec3 coords, uint m){
	float r = abs(sin(coords.x * 2) + sin(coords.y * 5) + sin(coords.z * 10)) * m;
	float g = abs(cos(coords.y * 2) * cos(coords.z * 5) * cos(coords.x * 10)) * m;
	float b = pow(pow(tan(coords.z * 2), tan(coords.z * 5)), tan(coords.y * 10)) * m;

	return vec3(abs(r) - floor(abs(r)), abs(g) - floor(abs(g)), abs(b) - floor(abs(b)));
}

vec3 pattern4(vec3 coords, uint m, float i){
	vec3 intervals = vec3(floor(abs(coords.x * i)), floor(abs(coords.y * i)), floor(abs(coords.z * i))) / i;

	vec4 texColor = modalTex(int(m), coords - intervals) * length(coords - intervals);
	return vec3(texColor) * 2;
}

vec3 pattern5(vec3 coords, uint m, float i){
	vec3 intervals1 = vec3(floor(abs(coords.x * i)), floor(abs(coords.y * i)), floor(abs(coords.z * i))) / i;
	vec3 intervals2 = vec3(floor(abs(intervals1.y * (i * 5))), floor(abs(intervals1.z * (i * 5))), floor(abs(intervals1.x * (i * 5)))) / (i * 5);
	vec3 intervals3 = vec3(floor(abs(intervals2.z * (i * 10))), floor(abs(intervals2.x * (i * 10))), floor(abs(intervals2.y * (i * 10)))) / (i * 10);

	vec4 texColor = modalTex(int(m), intervals3);
	return vec3(abs(texColor.r), abs(texColor.g), abs(texColor.b)) * 2;
}

vec3 pattern6(vec3 coords){
	vec3 weave = vec3(
		(coords.x * coords.x) + abs(coords.y * coords.x) + (coords.y * coords.y),
		(coords.z * coords.z) + abs(coords.z * coords.x) + (coords.x * coords.x),
		(coords.y * coords.y) + abs(coords.y * coords.z) + (coords.z * coords.z)
	);

	return weave;
}

vec3 pattern7(vec3 coords, uint m){
	vec3 weave = vec3(
		pow(coords.x, m) + abs(coords.y * coords.x * m) + pow(coords.y, m),
		pow(coords.z, m) + abs(coords.z * coords.x * m) + pow(coords.x, m),
		pow(coords.y, m) + abs(coords.y * coords.z * m) + pow(coords.z, m)
	);

	return weave;
}

vec4 pattern8(vec3 coords, uint m, double t){
	vec4 texColor1 = modalTex(int(m), coords * sin(float(t)));
	vec4 texColor2 = modalTex(int(m + 1), coords * cos(float(t)));
	vec4 texColor3 = modalTex(int(m + 2), coords * tan(float(t)));

	return (texColor1 + texColor2) * texColor3; // TODO: Make this a cool effect
}

vec4 pattern9(vec3 coords, uint m, double t){
	vec4 texColor1 = modalTex(int(m), coords * sin(float(t)));
	vec4 texColor2 = modalTex(int(m + 1), coords * cos(float(t)));
	vec4 texColor3 = modalTex(int(m + 2), coords * tan(float(t)));

	return texColor3 * dot(texColor1, texColor2);
}