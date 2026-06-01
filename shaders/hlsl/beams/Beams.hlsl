#ifdef INCLUDE_TEXTURES // Textured Lighting

float3 getAmbient_sampled(float3 lights[3][2], uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    float3 ambient = lights[0][1] * (0.25 + (0.05 * intensity));
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            ambient += (lights[l][1] * (0.25 + (0.05 * intensity))) * (1.0 / count);
    return ambient;
}

float3 getDiffuse_sampled(float3 lights[3][2], float3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    float3 diffuse = lights[0][1] * getDiffuse(lights[0][0], coords - offset) * 0.5 * intensity;
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            diffuse += (lights[l][1] * getDiffuse(lights[l][0], coords - offset) * 0.5 * intensity) * (1.0 / count);
    return diffuse;
}

float3 getSpecular_sampled(float3 lights[3][2], float3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    float3 specular = lights[0][1] * getSpecular(cam_pos, coords, float(intensity + 1) * 0.5);
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            specular += (lights[l][1] * getSpecular(cam_pos, coords, float(intensity + 1) * 0.5)) * (1.0 / count);
    return specular;
}

#else // Flat Lighting

float3 getAmbient_flat(float3 lights[3][2], uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    float3 ambient = lights[0][1] * (0.25 + (0.05 * intensity));
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            ambient += (lights[l][1] * (0.25 + (0.05 * intensity))) * (1.0 / count);
    return ambient;
}

float3 getDiffuse_flat(float3 lights[3][2], float3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    float3 diffuse = lights[0][1] * getDiffuse(lights[0][0], coords - offset) * 0.5 * intensity;
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            diffuse += (lights[l][1] * getDiffuse(lights[l][0], coords - offset) * 0.5 * intensity) * (1.0 / count);
    return diffuse;
}

float3 getSpecular_flat(float3 lights[3][2], float3 coords, uint intensity){
    uint count = getLightCount(abs(mode) % 10);
    float3 specular = lights[0][1] * getSpecular(cam_pos, coords, float(intensity + 1) * 0.5);
    if(count > 1) // combining lights
        for(uint l = 1; l < count; l++) 
            specular += (lights[l][1] * getSpecular(cam_pos, coords, float(intensity + 1) * 0.5)) * (1.0 / count);
    return specular;
}
#endif