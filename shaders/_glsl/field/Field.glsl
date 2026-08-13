vec3 field1(vec3 ctrlPoint, vec3 coords){
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field2(uint ctrlIndices[3], vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIndices[0]];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field3(vec3 ctrlPoint, vec3 coords){
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field4(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field5(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field6(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field7(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field8(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field9(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field10(uint ctrlIdx, vec3 coords){
	vec3 ctrlPoint = ctrlPoints[ctrlIdx];
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field11(vec3 ctrlPoint, vec3 coords){
	return vec3(abs(ctrlPoint.x - coords.x), abs(ctrlPoint.y - coords.y), abs(ctrlPoint.z - coords.z));
}

vec3 field12(vec3 ctrlPoint, vec3 coords, vec3 color){
	return vec3(abs(ctrlPoint.x - coords.x) * color.r, abs(ctrlPoint.y - coords.y) * color.g, abs(ctrlPoint.z - coords.z) * color.b);
}