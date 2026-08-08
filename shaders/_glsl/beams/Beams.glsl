#ifdef INCLUDE_TEXTURES

vec3 getAmbient_sampled(vec3 lights[3][2], vec3 texcoords, uint intensity){
    vec4 texColor = modalTex((abs(mode / 100)) % 10, texcoords);
    float i = intensity / 10.0;
    return smoothstep(texColor.rgb, lights[0][1], vec3(float(intensity / 10.0), float(intensity / 5.0), float(intensity / 2.0)));
}

vec3 getDiffuse_sampled(vec3 lights[3][2], vec3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    vec3 diffuse = lights[0][1] * getDiffuse(lights[0][0], coords - offset) * 0.5 * intensity;
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            diffuse += (lights[l][1] * getDiffuse(lights[l][0], coords - offset) * 0.5 * intensity) * (1.0 / count);
    return diffuse;
}

vec3 getSpecular_sampled(vec3 lights[3][2], vec3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    vec3 specular = lights[0][1] * getSpecular(lights[0][0], cam_pos.xyz, coords, float(intensity + 1) * 0.5);
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            specular += (lights[l][1] * getSpecular(lights[l][0], cam_pos.xyz, coords, float(intensity + 1) * 0.5)) * (1.0 / count);
    return specular;
}

#else // Flat Lighting

vec3 getAmbient_flat(vec3 lights[3][2], uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    vec3 ambient = lights[0][1] * (0.25 + (0.05 * intensity));
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            ambient += (lights[l][1] * (0.25 + (0.05 * intensity))) * (1.0 / count);
    return ambient;
}

vec3 getDiffuse_flat(vec3 lights[3][2], vec3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    vec3 diffuse = lights[0][1] * getDiffuse(lights[0][0], coords - offset) * 0.5 * intensity;
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            diffuse += (lights[l][1] * getDiffuse(lights[l][0], coords - offset) * 0.5 * intensity) * (1.0 / count);
    return diffuse;
}

vec3 getSpecular_flat(vec3 lights[3][2], vec3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    vec3 specular = lights[0][1] * getSpecular(lights[0][0], cam_pos.xyz, coords, float(intensity + 1) * 0.5);
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            specular += (lights[l][1] * getSpecular(lights[l][0], cam_pos.xyz, coords, float(intensity + 1) * 0.5)) * (1.0 / count);
    return specular;
}

#endif