float4 color_correct(float4 color){ // switch red and blue color values
	float t = color[0];
	color[0] = color[2]; color[2] = t;
	return color;
}

float3 color_range(float3 color){ // maps color to +- range
	return (color - float3(0.5F, 0.5F, 0.5F)) * 2;
}

#ifdef INCLUDE_TEXTURES

float4 modalTex(uint sampleMode, float3 texcoords){
    if(abs(sampleMode) % 8 == 1) return tex1.Sample(sampler1, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 8 == 2) return tex2.Sample(sampler2, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 8 == 3) return tex3.Sample(sampler3, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 8 == 4) return tex4.Sample(sampler4, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 8 == 5) return tex5.Sample(sampler5, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 8 == 6) return tex6.Sample(sampler6, float2(texcoords.x, texcoords.y));
    else if(abs(sampleMode) % 8 == 7) return tex7.Sample(sampler7, float2(texcoords.x, texcoords.y));
    else return baseTex.Sample(baseSampler, float2(texcoords.x, texcoords.y));
}

float4 antialias2D(float2 coords, Texture2D tex, SamplerState samp, float antialiasArea, float antialiasSteps){
	if(antialiasArea == 0.0 || antialiasSteps == 0) return tex.Sample(samp, coords);
	else { // antialiasing algorithm
		float4 texColor = tex.Sample(samp, coords);
		for(uint a = 0; a < antialiasSteps; a++){
			float f = (antialiasArea / antialiasSteps) * (a + 1);
			float4 nebrTexColors[8] = {
				tex.Sample(samp, coords + float2(f, 0.0)), tex.Sample(samp, coords + float2(-f, 0.0)), // left and right
				tex.Sample(samp, coords + float2(0.0, f)), tex.Sample(samp, coords + float2(0.0, -f)), // top and bottom
				tex.Sample(samp, coords + float2(f, f)), tex.Sample(samp, coords + float2(-f, -f)), // top right and bottom left
				tex.Sample(samp, coords + float2(-f, f)), tex.Sample(samp, coords + float2(f, -f)) // top left and bottom right
			};
			for(uint n = 0; n < 8; n++) texColor += nebrTexColors[n]; // total
			texColor *= 1.0 / 8; // average
		}
		return texColor;
	}
}

float4 antialias3D(float3 coords, Texture3D tex, SamplerState samp, float antialiasArea, float antialiasSteps){
	if(antialiasArea == 0.0 || antialiasSteps == 0) return tex.Sample(samp, coords);
	else {
		float4 texColor = tex.Sample(samp, coords);
		for(uint a = 0; a < antialiasSteps; a++){
			float f = (antialiasArea / antialiasSteps) * (a + 1);
			for(uint l = 0; l < 3; l++){
				float d = -f + (f * l);
				float4 nebrTexColors[9] = {
					tex.Sample(samp, coords + float3(0.0, 0.0, d)),
					tex.Sample(samp, coords + float3(f, 0.0, d)), tex.Sample(samp, coords + float3(-f, 0.0, d)), // left and right
					tex.Sample(samp, coords + float3(0.0, f, d)), tex.Sample(samp, coords + float3(0.0, -f, d)), // top and bottom
					tex.Sample(samp, coords + float3(f, f, d)), tex.Sample(samp, coords + float3(-f, -f, d)), // top right and bottom left
					tex.Sample(samp, coords + float3(-f, f, d)), tex.Sample(samp, coords + float3(f, -f, d)) // top left and bottom right
				};
				for(uint n = 0; n < 9; n++) texColor += nebrTexColors[n]; // total
				texColor *= 1.0 / 9; // average
			}
		}
		return texColor;
	}
}

#endif

float3 getRandColor(uint seed){
	double3 randColor = double3(double(seed) * 0.722433, double(seed) * 0.423512, double(seed) * 0.324561);

	for(uint iter = 0; iter < 1; iter++){
		randColor.x *= randColor.z * 3.527719;
		randColor.y *= randColor.x * 4.342534;
		randColor.z *= randColor.y * 2.834565;
	}

	return float3(randColor.x - floor(randColor.x), randColor.y - floor(randColor.y), randColor.z - floor(randColor.z));
}

float4 getStepColor(int id){
	float attenuation = floor(id / 6.0) * COLOR_INC;

	if(id % 6 == 0) return float4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (id % 6 == 1) return float4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (id % 6 == 2) return float4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (id % 6 == 3) return float4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (id % 6 == 4) return float4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (id % 6 == 5) return float4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return float4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}