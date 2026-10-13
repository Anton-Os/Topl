float4 field1(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint + coords;
	return float4(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z), 1.0);
}

float4 field2(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint * coords;
	return float4(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z), 1.0);
}

float4 field3(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint / coords;
	return float4(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z), 1.0);
}

float4 field4(float3 ctrlPoint, float3 coords){
	float3 relCoord = cross(ctrlPoint, coords);
	return float4(abs(relCoord.x), abs(relCoord.y), abs(relCoord.z), 1.0);
}

float4 field5(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint / coords;
	return float4(abs(relCoord.x / relCoord.y), abs(relCoord.y / relCoord.z), abs(relCoord.z / relCoord.x), 1.0);
}

float4 field6(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint / coords;
	return float4(pow(relCoord.x, relCoord.y), pow(relCoord.y, relCoord.z), pow(relCoord.z, relCoord.x), 1.0);
}

float4 field7(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint / coords;
	return float4(abs(sin(relCoord.x + relCoord.y)), cos(abs(relCoord.y + relCoord.z)), abs(tan(relCoord.z + relCoord.x)), 1.0);
}

float4 field8(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint / coords;
	return float4(pow(abs(relCoord.x), 5.0), pow(abs(relCoord.y), 5.0), pow(abs(relCoord.z), 5.0), 1.0);
}

float4 field9(float3 ctrlPoint, float3 coords){
	float3 relCoord = ctrlPoint / coords;
	return float4(abs(sin(relCoord.x / 5.0)), abs(cos(relCoord.y / 5.0)), abs(tan(relCoord.z / 5.0)), 1.0);
}