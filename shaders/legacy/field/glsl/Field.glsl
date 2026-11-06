// Distance Fields

vec3 dist_field1(vec3 ctrlPoint, vec3 coords){
	return vec3(
		length(ctrlPoint - coords),
		length(ctrlPoint + coords),
		length(ctrlPoint * coords)
	);
}

vec3 dist_field2(vec3 ctrlPoint, vec3 coords){
	return vec3(
		length(vec3(pow(ctrlPoint.x, abs(coords.x)), pow(ctrlPoint.y, abs(coords.y)), pow(ctrlPoint.z, abs(coords.z)))),
		length(vec3(pow(ctrlPoint.x, abs(coords.y)), pow(ctrlPoint.y, abs(coords.z)), pow(ctrlPoint.z, abs(coords.x)))),
		length(vec3(pow(ctrlPoint.x, abs(coords.z)), pow(ctrlPoint.y, abs(coords.x)), pow(ctrlPoint.z, abs(coords.y))))
	);
}

vec3 dist_field3(vec3 ctrlPoint, vec3 coords){
	return vec3(
		length(vec3(pow(ctrlPoint.x, abs(1.0 / coords.x)), pow(ctrlPoint.y, abs(1.0 / coords.y)), pow(ctrlPoint.z, abs(1.0 / coords.z)))),
		length(vec3(pow(ctrlPoint.x, abs(1.0 / coords.y)), pow(ctrlPoint.y, abs(1.0 / coords.z)), pow(ctrlPoint.z, abs(1.0 / coords.x)))),
		length(vec3(pow(ctrlPoint.x, abs(1.0 / coords.z)), pow(ctrlPoint.y, abs(1.0 / coords.x)), pow(ctrlPoint.z, abs(1.0 / coords.y))))
	);
}

// Cross Fields

vec3 cross_field1(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = cross(ctrlPoint, coords);
	return vec3(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z));
}

vec3 cross_field2(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = cross(ctrlPoint, coords);
	return vec3(pow(relCoord.x, abs(relCoord.y)), pow(relCoord.y, abs(relCoord.z)), pow(relCoord.z, abs(relCoord.x)));
}

vec3 cross_field3(vec3 ctrlPoint, vec3 coords){
	vec3 relCoord = cross(
		vec3(ctrlPoint.x + ctrlPoint.y, ctrlPoint.y + ctrlPoint.z, ctrlPoint.x + ctrlPoint.z), 
		vec3(coords.x - coords.y, coords.y - coords.z, coords.z - coords.x)
	);
	return vec3(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z));
}

// Trig Fields

vec3 trig_field1(vec3 nearPt, vec3 secPt, vec3 farPt, vec3 coords){
	return vec3(
		length(nearPt - coords),
		length(secPt - coords),
		length(farPt - coords)
	);
}

vec3 trig_field2(vec3 nearPt, vec3 secPt, vec3 farPt, vec3 coords){
	return smoothstep(nearPt, farPt, secPt - coords);
}

vec3 trig_field3(vec3 nearPt, vec3 secPt, vec3 farPt, vec3 coords){
	return smoothstep(
		vec3(0.0, 0.0, 0.0), 
		vec3(1.0, 1.0, 1.0), 
		vec3(
			abs(sin(nearPt.x - coords.x)), 
			abs(cos(secPt.y - coords.y)), 
			abs(tan(farPt.z - coords.z))
		)
	);
}