// Terrain height field, mirrored on the CPU by Terrain_VertexShader::getHeight()
// params[0]: x water level, y height scale, z noise frequency

float getWaterLevel(){ return params[0].x; }

float getTerrainHeight(float2 xz){
	float amp = (params[0].y > 0.0)? params[0].y : 9.0;
	float freq = (params[0].z > 0.0)? params[0].z : 0.045;
	float2 p = xz * freq;

	float base = fbm(p);
	float ridge = 1.0 - abs(valueNoise(p * 0.5 + float2(17.3, 9.1)) * 2.0 - 1.0);
	return (base * 0.7 + ridge * ridge * 0.45 - 0.45) * amp;
}
