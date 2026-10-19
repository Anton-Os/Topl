// Modern Pixel: normals, lighting, sky, fog and tonemapping shared by modern pixel shaders
// Functions take the interpolated pixel input since HLSL has no global varyings

// Normals

float3 faceCamera(float3 n, float3 worldPos){ return (dot(n, cam_pos.xyz - worldPos) < 0.0)? -n : n; }

float3 getFacetNormal(float3 worldPos){ // flat normal from screen space derivatives, works for any displaced geometry
	float3 n = cross(ddx(worldPos), ddy(worldPos));
	return (dot(n, n) > 1e-14)? faceCamera(normalize(n), worldPos) : float3(0.0, 1.0, 0.0);
}

float3 getSurfaceNormal(PS_INPUT input, bool isFlat){
	float3 facet = getFacetNormal(input.world_pos); // derivatives are taken outside of any branch
	if(isFlat || dot(input.world_normal, input.world_normal) < 0.0001) return facet;
	return faceCamera(normalize(input.world_normal), input.world_pos);
}

// Lighting

float3 getLightDir(float3 p){ return (light_pos.w < 0.0)? normalize(light_pos.xyz) : normalize(light_pos.xyz - p); }

float getLightAtten(float3 p){
	if(light_pos.w < 0.0) return abs(light_pos.w);
	float d = length(light_pos.xyz - p);
	return light_pos.w / (1.0 + 0.045 * d + 0.0075 * d * d);
}

float3 getSunDir(){ return (light_pos.w < 0.0)? normalize(light_pos.xyz) : normalize(light_pos.xyz - cam_pos.xyz); }

float3 getAmbient(float3 n){ // hemispheric ambient from sky and ground tones
	float3 skyTone = lerp(fog_color.rgb, float3(0.35, 0.5, 0.8), 0.4);
	float3 groundTone = fog_color.rgb * float3(0.45, 0.4, 0.35);
	return lerp(groundTone, skyTone, n.y * 0.5 + 0.5) * light_color.w;
}

float getFresnel(float3 n, float3 v, float power){ return pow(1.0 - saturate(dot(n, v)), power); }

float3 getSkyColor(float3 dir){
	dir = normalize(dir);
	float3 sun = getSunDir();
	float height = clamp(dir.y, -1.0, 1.0);

	float3 zenith = lerp(float3(0.08, 0.22, 0.55), fog_color.rgb * 0.35, 0.25);
	float3 sky = lerp(fog_color.rgb, zenith, pow(max(height, 0.0), 0.55));
	sky = lerp(sky, fog_color.rgb * 0.55, saturate(-height * 3.0)); // below horizon

	float sunAmount = max(dot(dir, sun), 0.0);
	sky += light_color.rgb * (pow(sunAmount, 900.0) * 6.0 + pow(sunAmount, 12.0) * 0.35); // disk and glow
	return sky;
}

float3 shadeBlinnPhong(float3 albedo, float3 n, float3 p, float shininess, float specStrength){
	float3 l = getLightDir(p);
	float3 v = normalize(cam_pos.xyz - p);
	float3 h = normalize(l + v);
	float atten = getLightAtten(p);

	float diffuse = max(dot(n, l), 0.0);
	float specular = (diffuse > 0.0)? pow(max(dot(n, h), 0.0), shininess) * specStrength : 0.0;
	return albedo * getAmbient(n) + (albedo * diffuse + specular.xxx) * light_color.rgb * atten;
}

// Post

float3 applyFog(float3 color, float3 p){
	float dist = length(p - cam_pos.xyz);
	float amount = 1.0 - exp(-dist * fog_color.w);
	return lerp(color, getSkyColor(p - cam_pos.xyz), saturate(amount));
}

float3 toneMap(float3 c){ // ACES filmic approximation followed by gamma correction
	c = max(c, float3(0.0, 0.0, 0.0));
	c = (c * (2.51 * c + 0.03)) / (c * (2.43 * c + 0.59) + 0.14);
	return pow(saturate(c), 1.0 / 2.2);
}

float4 getActorColor(){ return actor_color; } // tint assigned per actor from Modern_VertexShader::setActorColor()

float3 applyTexture(float3 albedo, PS_INPUT input){ // blends the actor texture over a color, actor_tex.x of 0 leaves it untouched
#ifdef INCLUDE_TEXTURES
	float2 uv = (actor_tex.w > 0.5 && actor_tex.w < 1.5)? input.world_pos.xz * 0.25 : input.texcoord.xy;
	uv = uv * ((actor_tex.y > 0.0)? actor_tex.y : 1.0) + float2(getTime() * actor_tex.z, 0.0);
	float3 texColor = pow(abs(baseTex.Sample(baseSampler, uv).rgb), 2.2); // texture is stored in gamma space
	return lerp(albedo, texColor * (0.35 + albedo * 0.65), saturate(actor_tex.x));
#else
	return albedo;
#endif
}

float applyTextureAlpha(float alpha, PS_INPUT input){ // sprites (actor_tex.w of 2) take their coverage from the texture alpha
#ifdef INCLUDE_TEXTURES
	float coverage = baseTex.Sample(baseSampler, input.texcoord.xy).a; // sampled outside the branch to keep derivatives valid
	return (actor_tex.w > 1.5)? alpha * coverage : alpha;
#else
	return alpha;
#endif
}
