vec4 color_correct(vec4 color){ // switch red and blue color values
    float t = color.r;
    color.r = color.b; color.b = t;
    return color;
}

vec3 color_range(vec3 color){ // maps color to +- range
	return (color - vec3(0.5F, 0.5F, 0.5F)) * 2;
}

#ifdef INCLUDE_TEXTURES

vec4 modalTex(int m, vec3 texcoord){
    vec4 color;
    if(abs(m) % 10 == 8) color = texture(volumeTex, texcoord); // volumetric texture
    else if(abs(m) % 10 == 9) color = texture(volumeTex, vec3(texcoord.x, texcoord.y, SLICE)); // volumetric slice
    else { // select texture
        if(abs(m) % 10 == 1) color = texture(tex1, vec2(texcoord.x, texcoord.y));
        else if(abs(m) % 10 == 2) color = texture(tex2, vec2(texcoord.x, texcoord.y));
        else if(abs(m) % 10 == 3) color = texture(tex3, vec2(texcoord.x, texcoord.y));
        else if(abs(m) % 10 == 4) color = texture(tex4, vec2(texcoord.x, texcoord.y));
        else if(abs(m) % 10 == 5) color = texture(tex5, vec2(texcoord.x, texcoord.y));
        else if(abs(m) % 10 == 6) color = texture(tex6, vec2(texcoord.x, texcoord.y));
        else if(abs(m) % 10 == 7) color = texture(tex7, vec2(texcoord.x, texcoord.y));
        else color = texture(baseTex, vec2(texcoord.x, texcoord.y)); // base texture
    }
    return color;
}

vec4 antialias2D(vec2 coords, sampler2D tex2D, float antialiasArea, float antialiasSteps){
    if(antialiasArea == 0.0 || antialiasSteps == 0) return texture(tex2D, coords);
    else { // antialiasing algorithm
        vec4 texColor = texture(tex2D, coords);
        for(uint a = 0; a < antialiasSteps; a++){
            float f = (antialiasArea / antialiasSteps) * (a + 1);
            vec4 nebrTexColors[8] = {
                texture(tex2D, coords + vec2(f, 0.0)), texture(tex2D, coords + vec2(-f, 0.0)), // left and right
                texture(tex2D, coords + vec2(0.0, f)), texture(tex2D, coords + vec2(0.0, -f)), // top and bottom
                texture(tex2D, coords + vec2(f, f)), texture(tex2D, coords + vec2(-f, -f)), // top right and bottom left
                texture(tex2D, coords + vec2(-f, f)), texture(tex2D, coords + vec2(f, -f)) // top left and bottom right
            };
            for(uint n = 0; n < 8; n++) texColor += nebrTexColors[n]; // total
            texColor *= 1.0 / 8; // average
        }
        return texColor;
    }
}

vec4 antialias3D(vec3 coords, sampler3D tex3D, float antialiasArea, float antialiasSteps){
    if(antialiasArea == 0.0 || antialiasSteps == 0) return texture(tex3D, coords);
    else {
        vec4 texColor = texture(tex3D, coords);
        for(uint a = 0; a < antialiasSteps; a++){
            float f = (antialiasArea / antialiasSteps) * (a + 1);
            for(uint l = 0; l < 3; l++){
                float d = -f + (f * l);
                vec4 nebrTexColors[9] = {
                    texture(tex3D, coords + vec3(0.0, 0.0, d)),
                    texture(tex3D, coords + vec3(f, 0.0, d)), texture(tex3D, coords + vec3(-f, 0.0, d)), // left and right
                    texture(tex3D, coords + vec3(0.0, f, d)), texture(tex3D, coords + vec3(0.0, -f, d)), // top and bottom
                    texture(tex3D, coords + vec3(f, f, d)), texture(tex3D, coords + vec3(-f, -f, d)), // top right and bottom left
                    texture(tex3D, coords + vec3(-f, f, d)), texture(tex3D, coords + vec3(f, -f, d)) // top left and bottom right
                };
                for(uint n = 0; n < 9; n++) texColor += nebrTexColors[n]; // total
                    texColor *= 1.0 / 9; // average
            }
        }
        return texColor;
    }
}
#endif

/* vec4 getRandColor(vec4 seedColor){
	vec4 randColor = seedColor * vec4(34.234, 11.559, 81.344, 56.34);

	while(randColor.x > 1.0) randColor.r -= pow(randColor.x, 0.5);213
	while(randColor.y > 1.0) randColor.g -= pow(randColor.y, 0.5);
	while(randColor.z > 1.0) randColor.b -= pow(randColor.z, 0.5);
	while(randColor.a > 1.0) randColor.a -= pow(randColor.a, 0.5);

	return randColor;
} */

vec3 getRandColor(uint seed){
	dvec3 randColor = dvec3(double(seed)* 0.325243, double(seed) * 0.953254, double(seed) * 0.563445);

	for(uint iter = 0; iter < 1; iter++){
		randColor.x += randColor.y * 1.634923;
		randColor.y += randColor.z * 2.456431;
		randColor.z += randColor.x * 3.123145;
	}

	randColor = dvec3(randColor.x - floor(randColor.x), randColor.y - floor(randColor.y), randColor.z - floor(randColor.z));
	return vec3(float(randColor.x), float(randColor.y), float(randColor.z));
}

vec4 getStepColor(uint index){
	float attenuation = floor(index / 6.0) * COLOR_INC;

	if(index % 6 == 0) return vec4(1.0 - attenuation, 0.0, 0.0, 1.0); // red
	else if (index % 6 == 1) return vec4(0.0, 1.0 - attenuation, 0.0, 1.0); // green
	else if (index % 6 == 2) return vec4(0.0, 0.0, 1.0 - attenuation, 1.0); // blue
	else if (index % 6 == 3) return vec4(1.0 - attenuation, 1.0 - attenuation, 0.0, 1.0); // yellow
	else if (index % 6 == 4) return vec4(0.0, 1.0 - attenuation, 1.0 - attenuation, 1.0); // cyan
	else if (index % 6 == 5) return vec4(1.0 - attenuation, 0.0, 1.0 - attenuation, 1.0); // magenta
	else return vec4(1.0 - attenuation, 1.0 - attenuation, 1.0 - attenuation, 1.0); // white
}