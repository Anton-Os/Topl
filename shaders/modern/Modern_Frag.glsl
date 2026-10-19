// Modern Frag: normals, lighting, sky, fog and tonemapping shared by modern fragment shaders

layout(location = 0) out vec4 color_final;

// Normals

vec3 faceCamera(vec3 n){ return (dot(n, cam_pos.xyz - world_pos) < 0.0)? -n : n; }

vec3 getFacetNormal(){ // flat normal from screen space derivatives, works for any displaced geometry
	vec3 n = cross(dFdx(world_pos), dFdy(world_pos));
	return (dot(n, n) > 1e-14)? faceCamera(normalize(n)) : vec3(0.0, 1.0, 0.0);
}

vec3 getSurfaceNormal(bool isFlat){
	vec3 facet = getFacetNormal(); // derivatives are taken outside of any branch
	if(isFlat || dot(world_normal, world_normal) < 0.0001) return facet;
	return faceCamera(normalize(world_normal));
}

// Lighting

vec3 getLightDir(vec3 p){ return (light_pos.w < 0.0)? normalize(light_pos.xyz) : normalize(light_pos.xyz - p); }

float getLightAtten(vec3 p){
	if(light_pos.w < 0.0) return abs(light_pos.w);
	float d = length(light_pos.xyz - p);
	return light_pos.w / (1.0 + 0.045 * d + 0.0075 * d * d);
}

vec3 getSunDir(){ return (light_pos.w < 0.0)? normalize(light_pos.xyz) : normalize(light_pos.xyz - cam_pos.xyz); }

vec3 getAmbient(vec3 n){ // hemispheric ambient from sky and ground tones
	vec3 skyTone = mix(fog_color.rgb, vec3(0.35, 0.5, 0.8), 0.4);
	vec3 groundTone = fog_color.rgb * vec3(0.45, 0.4, 0.35);
	return mix(groundTone, skyTone, n.y * 0.5 + 0.5) * light_color.w;
}

float getFresnel(vec3 n, vec3 v, float power){ return pow(1.0 - clamp(dot(n, v), 0.0, 1.0), power); }

vec3 getSkyColor(vec3 dir){
	dir = normalize(dir);
	vec3 sun = getSunDir();
	float height = clamp(dir.y, -1.0, 1.0);

	vec3 zenith = mix(vec3(0.08, 0.22, 0.55), fog_color.rgb * 0.35, 0.25);
	vec3 sky = mix(fog_color.rgb, zenith, pow(max(height, 0.0), 0.55));
	sky = mix(sky, fog_color.rgb * 0.55, clamp(-height * 3.0, 0.0, 1.0)); // below horizon

	float sunAmount = max(dot(dir, sun), 0.0);
	sky += light_color.rgb * (pow(sunAmount, 900.0) * 6.0 + pow(sunAmount, 12.0) * 0.35); // disk and glow
	return sky;
}

vec3 shadeBlinnPhong(vec3 albedo, vec3 n, vec3 p, float shininess, float specStrength){
	vec3 l = getLightDir(p);
	vec3 v = normalize(cam_pos.xyz - p);
	vec3 h = normalize(l + v);
	float atten = getLightAtten(p);

	float diffuse = max(dot(n, l), 0.0);
	float specular = (diffuse > 0.0)? pow(max(dot(n, h), 0.0), shininess) * specStrength : 0.0;
	return albedo * getAmbient(n) + (albedo * diffuse + vec3(specular)) * light_color.rgb * atten;
}

// Post

vec3 applyFog(vec3 color, vec3 p){
	float dist = length(p - cam_pos.xyz);
	float amount = 1.0 - exp(-dist * fog_color.w);
	return mix(color, getSkyColor(p - cam_pos.xyz), clamp(amount, 0.0, 1.0));
}

vec3 toneMap(vec3 c){ // ACES filmic approximation followed by gamma correction
	c = max(c, vec3(0.0));
	c = (c * (2.51 * c + 0.03)) / (c * (2.43 * c + 0.59) + 0.14);
	return pow(clamp(c, 0.0, 1.0), vec3(1.0 / 2.2));
}

vec4 getActorColor(){ return actor_color; } // tint assigned per actor from Modern_VertexShader::setActorColor()

vec3 applyTexture(vec3 albedo){ // blends the actor texture over a color, actor_tex.x of 0 leaves it untouched
#ifdef INCLUDE_TEXTURES
	vec2 uv = (actor_tex.w > 0.5 && actor_tex.w < 1.5)? world_pos.xz * 0.25 : texcoord.xy;
	uv = uv * ((actor_tex.y > 0.0)? actor_tex.y : 1.0) + vec2(getTime() * actor_tex.z, 0.0);
	vec3 texColor = pow(texture(baseTex, uv).rgb, vec3(2.2)); // texture is stored in gamma space
	return mix(albedo, texColor * (0.35 + albedo * 0.65), clamp(actor_tex.x, 0.0, 1.0));
#else
	return albedo;
#endif
}

float applyTextureAlpha(float alpha){ // sprites (actor_tex.w of 2) take their coverage from the texture alpha
#ifdef INCLUDE_TEXTURES
	float coverage = texture(baseTex, texcoord.xy).a; // sampled outside the branch to keep derivatives valid
	return (actor_tex.w > 1.5)? alpha * coverage : alpha;
#else
	return alpha;
#endif
}
