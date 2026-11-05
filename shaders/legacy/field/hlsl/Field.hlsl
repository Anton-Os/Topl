// Distance Fields

float4 field1(float3 ctrlPoint, float3 coords){
	return float4(
		length(ctrlPoint - coords),
		length(ctrlPoint + coords),
		length(ctrlPoint * coords),
		1.0
	);
}

float4 field2(float3 ctrlPoint, float3 coords){
	return float4(
		length(float3(pow(ctrlPoint.x, abs(coords.x)), pow(ctrlPoint.y, abs(coords.y)), pow(ctrlPoint.z, abs(coords.z)))),
		length(float3(pow(ctrlPoint.x, abs(coords.y)), pow(ctrlPoint.y, abs(coords.z)), pow(ctrlPoint.z, abs(coords.x)))),
		length(float3(pow(ctrlPoint.x, abs(coords.z)), pow(ctrlPoint.y, abs(coords.x)), pow(ctrlPoint.z, abs(coords.y)))),
		1.0
	);
}

float4 field3(float3 ctrlPoint, float3 coords){
	return float4(
		length(float3(pow(ctrlPoint.x, abs(1.0 / coords.x)), pow(ctrlPoint.y, abs(1.0 / coords.y)), pow(ctrlPoint.z, abs(1.0 / coords.z)))),
		length(float3(pow(ctrlPoint.x, abs(1.0 / coords.y)), pow(ctrlPoint.y, abs(1.0 / coords.z)), pow(ctrlPoint.z, abs(1.0 / coords.x)))),
		length(float3(pow(ctrlPoint.x, abs(1.0 / coords.z)), pow(ctrlPoint.y, abs(1.0 / coords.x)), pow(ctrlPoint.z, abs(1.0 / coords.y)))),
		1.0
	);
}

// Cross Fields

float4 field4(float3 ctrlPoint, float3 coords){
	float3 relCoord = cross(ctrlPoint, coords);
	return float4(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z), 1.0);
}

float4 field5(float3 ctrlPoint, float3 coords){
	float3 relCoord = cross(ctrlPoint, coords);
	return float4(pow(relCoord.x, abs(relCoord.y)), pow(relCoord.y, abs(relCoord.z)), pow(relCoord.z, abs(relCoord.x)), 1.0);
}

float4 field6(float3 ctrlPoint, float3 coords){
	float3 relCoord = cross(
		float3(ctrlPoint.x + ctrlPoint.y, ctrlPoint.y + ctrlPoint.z, ctrlPoint.x + ctrlPoint.z), 
		float3(coords.x - coords.y, coords.y - coords.z, coords.z - coords.x)
	);
	return float4(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z), 1.0);
}

// Trig Fields

float4 field7(float3 nearPt, float3 secPt, float3 farPt, float3 coords){
	return float4(
		length(nearPt - coords) + length(nearPt + coords),
		length(secPt - coords) + length(secPt + coords),
		length(farPt - coords) + length(farPt + coords),
		1.0
	);
}

float4 field8(float3 nearPt, float3 secPt, float3 farPt, float3 coords){
	return float4(smoothstep(
		nearPt * length(farPt), 
		secPt * length(farPt), 
		coords
	), 1.0);
}

float4 field9(float3 nearPt, float3 secPt, float3 farPt, float3 coords){
	return float4(smoothstep(
		float3(0.0, 0.0, 0.0), 
		float3(1.0, 1.0, 1.0), 
		float3(
			abs(sin(nearPt.x - coords.x) * dot(nearPt, coords)), 
			abs(cos(secPt.y - coords.y) * dot(secPt, coords)), 
			abs(tan(farPt.z - coords.z) * dot(farPt, coords))
		)
	), 1.0);
}