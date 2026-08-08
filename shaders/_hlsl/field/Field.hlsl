float4 field1(float3 ctrlPoint, float3 coords){
	float l = length(coords - ctrlPoint);
	return float4(l, l, l, 1.0);
	// return float4(length(coords), length(ctrlPoint), length(coords - ctrlPoint), 1.0);
}

float4 field2(float3 ctrlPoint, float3 coords){
	float d = dot(coords, ctrlPoint);
	return float4(d, d, d, 1.0);
	// return float4(dot(coords, ctrlPoint), length(coords / ctrlPoint), length(ctrlPoint / coords), 1.0);
}

float4 field3(float3 ctrlPoint, float3 coords){
	float l = length(coords - ctrlPoint);
	float d = dot(coords, ctrlPoint);
	return float4(abs(l + d), abs(d / l), abs(l - d), 1.0);
	// return float4(cross(coords, ctrlPoint), 1.0);
}

float4 field4(float3 ctrlPoint, float3 coords){
	float l = length(coords - ctrlPoint);
	float d = dot(coords, ctrlPoint);
	return float4(sin((l + d) * 10), cos((l * d) * 10), tan((l / d) * 10), 1.0);
	// return float4(cross(coords, ctrlPoint) + cross(-ctrlPoint, coords), 1.0);
}

float4 field5(float3 ctrlPoint, float3 coords){
	return float4(pow(coords.x, abs(ctrlPoint.x)), pow(coords.y, abs(ctrlPoint.y)), pow(coords.z, abs(ctrlPoint.z)), 1.0);
	//return float4(pow(coords.x, nearIdx), pow(coords.y, nearIdx), pow(coords.z, nearIdx), 1.0);
}

float4 field6(uint nearIdx, uint secondIdx, uint farIdx, float3 coords){
	return float4(length(coords - ctrlPoints[nearIdx]), length(coords - ctrlPoints[secondIdx]), length(coords - ctrlPoints[farIdx]), 1.0);
	// return float4(sin(coords.x * nearIdx), cos(coords.y * nearIdx), tan(coords.z * nearIdx), 1.0);
}

float4 field7(uint nearIdx, uint secondIdx, uint farIdx, float3 coords){
	return float4(dot(coords, ctrlPoints[nearIdx]), dot(coords,  ctrlPoints[secondIdx]), dot(coords, ctrlPoints[farIdx]), 1.0);
	// return float4((coords.x + coords.z) * nearIdx, (coords.y - coords.z) / nearIdx, length(coords), 1.0);
}

float4 field8(uint nearIdx, uint secondIdx, uint farIdx, float3 coords){
	return float4(ctrlPoints[nearIdx].x, ctrlPoints[(nearIdx + 1) % 8].y, ctrlPoints[(nearIdx - 1) % 8].z, 1.0);
}

float4 field9(uint nearIdx, uint secondIdx, uint farIdx, float3 coords){
	return float4(abs(coords.x * nearIdx) - abs(floor(coords.x * nearIdx)), abs(ceil(coords.y * nearIdx)) - abs(coords.y * nearIdx), dot(coords, ctrlPoints[nearIdx]), 1.0);
}