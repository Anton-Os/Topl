vec3 getAmbient_sampled(vec3 lights[3][2], vec3 texcoords, uint intensity){
    vec4 texColor = modalTex((abs(mode / 100)) % 10, texcoords);
    float i = intensity / 10.0;
    return smoothstep(texColor.rgb, lights[0][1], vec3(i, i, i));
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