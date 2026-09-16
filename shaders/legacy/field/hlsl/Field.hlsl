float4 field1(float3 ctrlPoint, float3 coords){
	return float4(ctrlPoint - coords, 1.0);
}

float4 field2(float3 ctrlPoint, float3 coords){
	return float4(ctrlPoint - coords, 1.0);
}

float4 field3(float3 ctrlPoint, float3 coords){
	return float4(ctrlPoint - coords, 1.0);
}

float4 field4(float3 ctrlPoint, float3 coords){
	return float4(ctrlPoint - coords, 1.0);	
}

float4 field5(float3 ctrlPoint, float3 coords){
	return float4(ctrlPoint - coords, 1.0);
}

float4 field6(uint nIdx, uint sIdx, uint fIdx, float3 coords){
	float3 ctrlPoint = ctrlPoints[nIdx];
	return float4(ctrlPoint - coords, 1.0);
}

float4 field7(uint nIdx, uint sIdx, uint fIdx, float3 coords){
	float3 ctrlPoint = ctrlPoints[sIdx];
	return float4(ctrlPoint - coords, 1.0);	
}

float4 field8(uint nIdx, uint sIdx, uint fIdx, float3 coords){
	float3 ctrlPoint = ctrlPoints[fIdx];
	return float4(ctrlPoint - coords, 1.0);	
}

float4 field9(uint nIdx, uint sIdx, uint fIdx, float3 coords){
	float3 ctrlPoint = ctrlPoints[nIdx] - ctrlPoints[sIdx] - ctrlPoints[fIdx];
	return float4(ctrlPoint - coords, 1.0);
}