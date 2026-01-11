float4 coordField(float3 ctrlPoint, float3 coords){
	float dist = length(ctrlPoint - coords);
	float size = abs(mode % 100) * FIELD_SIZE;

	return float4(float3(dist * abs(ctrlPoint.r), dist * abs(ctrlPoint.g), dist * abs(ctrlPoint.b)) * size, 1.0);
}

float4 trigField(float3 ctrlPoint, float3 coords, float3 color){
	float distance = length(coords - ctrlPoint);
	float size = abs(mode % 100) * FIELD_SIZE;

	ctrlPoint *= float3(abs(sin(float(timeElapse) / 1000)), abs(cos(float(timeElapse) / 1000)), abs(tan(float(timeElapse) / 1000))) * (1.0 / size);
	// else ctrlPoint /= (float(timeElapse) / 1000) * size;

	ctrlPoint += float3(abs(mode % 100) / distance, abs(mode % 100) / distance, abs(mode % 100) / distance);

	return float4(ctrlPoint * color, 1.0);
}

float4 centerField(float3 ctrlPoint, float3 coords){
	float3 relCoord = coords - ctrlPoint;
	float3 angles = float3(atan(relCoord.y / relCoord.x), atan(relCoord.x / relCoord.z), atan(relCoord.z / relCoord.y));

	return float4(ceil(angles.r) - angles.r, ceil(angles.g) - angles.g, ceil(angles.b) - angles.b, 1.0) * abs(mode % 100);
}

float4 proximaField(float3 ctrlPoint, float3 coords){
	float3 relCoord = coords - ctrlPoint;
	for(uint c = 0; c < 8; c++) 
		if(ctrlPoints[c].x != ctrlPoint.x && ctrlPoints[c].y != ctrlPoint.y && ctrlPoints[c].z != ctrlPoint.z)
			relCoord += (coords - ctrlPoint) / abs(mode % 100);

	return float4(relCoord, 1.0);
}

float4 neonField(uint ctrlIdx, float3 coords){
	float3 relCoord = ctrlPoints[ctrlIdx] - coords;
	float r = pow(abs(relCoord.x), abs(relCoord.y));
	float g = pow(abs(relCoord.x), abs(relCoord.z));
	float b = pow(abs(relCoord.z), abs(relCoord.y));

	return float4((relCoord * float3(r, g, b)) / abs(mode % 100), 1.0);
}

float4 gradField(uint ctrlIdx, float3 coords){
	float3 relCoord = ctrlPoints[ctrlIdx] - coords;
	for(uint c = 0; c < 8; c++) if(c != ctrlIdx) relCoord += (ctrlPoints[c] - coords) * abs(cos(pow(c, (abs(mode) % 100) + 1)));

	// return float4(relCoord * ((timeElapse * 5) - floor(timeElapse * 5)), 1.0);
	return float4(relCoord, 1.0);
}

float4 crossField(uint ctrlIdx, float3 coords){
	float3 coord1 = ctrlPoints[ctrlIdx] - coords;
	float3 coord2 = ctrlPoints[8 - ctrlIdx - 1] + coords;

	return float4(cross(coord1, coord2) / ((abs(mode) % 100) + 1), 1.0);
}

float4 flowField(uint ctrlIdx, float3 coords){
	float3 nearestPoint = ctrlPoints[ctrlIdx] - coords;
	float3 secondPoint = float3(10000.0, 10000.0, 10000.0);
	for(uint c = 0; c < 8; c++)
		if(c != ctrlIdx && distance(ctrlPoints[c] - coords, nearestPoint) < distance(secondPoint, nearestPoint))
			secondPoint = ctrlPoints[c] - coords;

	if(abs(length(nearestPoint - secondPoint)) < length(coords))
		return float4((nearestPoint - secondPoint) * distance(nearestPoint, coords) * ((abs(mode) % 100) + 1), 1.0);
	else return float4(1.0, 1.0, 1.0, 0.15);
}