vec3 field1(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint + coords;
	return vec3(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z));
}

vec3 field2(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint * coords;
	return vec3(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z));
}

vec3 field3(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint / coords;
	return vec3(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z));
}

vec3 field4(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = cross(ctrlPoint, coords);
	return vec3(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z));
}

vec3 field5(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	return vec3(abs(relCoord.x - relCoord.y), abs(relCoord.y - relCoord.z), abs(relCoord.z - relCoord.x));
}

vec3 field6(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	return vec3(abs(relCoord.x * relCoord.y), abs(relCoord.y * relCoord.z), abs(relCoord.z * relCoord.x));
}

vec3 field7(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	return vec3(abs(relCoord.x / relCoord.y), abs(relCoord.y / relCoord.z), abs(relCoord.z / relCoord.x));
}

vec3 field8(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	return vec3(pow(abs(relCoord.x), length(relCoord)), pow(abs(relCoord.y), length(relCoord)), pow(abs(relCoord.z), length(relCoord)));
}

vec3 field9(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = ctrlPoint - coords;
	return vec3(abs(sin(relCoord.x / length(relCoord))), abs(cos(relCoord.y / length(relCoord))), abs(tan(relCoord.z / length(relCoord))));
}