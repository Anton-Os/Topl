float4 field1(float3 ctrlPoint, float3 coords){
	return float4(coords - ctrlPoint, 1.0);
}

float4 field2(float3 ctrlPoint, float3 coords, float3 color){
	return float4(ctrlPoint / (coords + color), 1.0);
}

float4 field3(float3 ctrlPoint, float3 coords){
	return float4(cross(coords, ctrlPoint), 1.0);
}

float4 field4(float3 ctrlPoint, float3 coords){
	return float4(cross(coords, ctrlPoint) + cross(-ctrlPoint, coords), 1.0);
}

float4 field5(uint ctrlIdx, float3 coords){
	return float4(pow(coords.x, ctrlIdx), pow(coords.y, ctrlIdx), pow(coords.z, ctrlIdx), 1.0);
}

float4 field6(uint ctrlIdx, float3 coords){
	return float4(sin(coords.x * ctrlIdx), cos(coords.y * ctrlIdx), tan(coords.z * ctrlIdx), 1.0);
}

float4 field7(uint ctrlIdx, float3 coords){
	return float4((coords.x + coords.z) * ctrlIdx, (coords.y - coords.z) / ctrlIdx, length(coords), 1.0);
}

float4 field8(uint ctrlIdx, float3 coords){
	return float4(ctrlPoints[ctrlIdx].x, ctrlPoints[(ctrlIdx + 1) % 8].y, ctrlPoints[(ctrlIdx - 1) % 8].z, 1.0);
}

float4 field9(uint ctrlIdx, float3 coords){
	return float4(abs(coords.x * ctrlIdx) - abs(floor(coords.x * ctrlIdx)), abs(ceil(coords.y * ctrlIdx)) - abs(coords.y * ctrlIdx), dot(coords, ctrlPoints[ctrlIdx]), 1.0);
}