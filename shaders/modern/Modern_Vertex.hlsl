// Modern Vertex: actor transforms and the FPS camera

// FPS Camera
// Yaw turns around the world up axis, pitch tilts the view, roll banks it. Yaw 0 looks down +Z

float getCamZoom(){ return (cam_pos.w > 0.0)? cam_pos.w : 1.0; }
float getCamFov(){ return (cam_lens.x > 0.0)? cam_lens.x : 1.0472; }
float getCamAspect(){ return (cam_lens.y > 0.0)? cam_lens.y : 1.0; }
float getCamNear(){ return (cam_lens.z > 0.0)? cam_lens.z : 0.05; }
float getCamFar(){ return (cam_lens.w > getCamNear())? cam_lens.w : 400.0; }
float getCamFocal(){ return getCamZoom() / tan(getCamFov() * 0.5); }

void fpsBasis(out float3 right, out float3 up, out float3 forward){
	float yaw = look_pos.x;
	float pitch = clamp(look_pos.y, -1.55, 1.55);
	float roll = look_pos.z;

	forward = float3(sin(yaw) * cos(pitch), sin(pitch), cos(yaw) * cos(pitch));
	float3 flatRight = float3(cos(yaw), 0.0, -sin(yaw));
	float3 flatUp = cross(forward, flatRight);

	right = flatRight * cos(roll) + flatUp * sin(roll);
	up = flatUp * cos(roll) - flatRight * sin(roll);
}

float3 fpsView(float3 worldPos){ // world space into camera space (x right, y up, z forward)
	float3 right, up, forward;
	fpsBasis(right, up, forward);
	float3 rel = worldPos - cam_pos.xyz;
	return float3(dot(rel, right), dot(rel, up), dot(rel, forward));
}

float4 fpsProject(float3 viewPos){ // perspective projection into DirectX clip space (z from 0 to 1)
	float n = getCamNear();
	float f = getCamFar();
	float focal = getCamFocal();
	return float4(
		viewPos.x * focal / getCamAspect(),
		viewPos.y * focal,
		viewPos.z * (f / (f - n)) - ((f * n) / (f - n)),
		viewPos.z
	);
}

float4 fpsTransform(float3 worldPos){ return fpsProject(fpsView(worldPos)); }

float3 fpsRay(float2 ndc){ // world space ray through a point on screen
	float3 right, up, forward;
	fpsBasis(right, up, forward);
	float focal = getCamFocal();
	return forward + right * (ndc.x * getCamAspect() / focal) + up * (ndc.y / focal);
}

// Actor Transforms

float3 getWorldPos(float3 localPos){ return actorRotate(localPos * scale, rotation) + offset; }

float3 getWorldNormal(float3 localNormal){
	float3 s = max(abs(scale), float3(0.0001, 0.0001, 0.0001));
	return actorRotate(localNormal / s, rotation);
}

// Engine meshes do not carry reliable normals, so modern shaders use facet normals from derivatives in the
// pixel stage. Actors flagged with actor_params.w > 0.5 are treated as spheres and get smooth radial normals
float3 getModelNormal(float3 localPos){
	if(actor_params.w <= 0.5 || dot(localPos, localPos) < 1e-10) return float3(0.0, 0.0, 0.0); // zero selects facet normals
	return getWorldNormal(normalize(localPos));
}

VS_OUTPUT passVaryings(VS_INPUT input, float3 worldPos, float3 worldNormal){
	VS_OUTPUT output;
	output.pos = fpsTransform(worldPos);
	output.world_pos = worldPos;
	output.world_normal = worldNormal;
	output.texcoord = input.texcoord;
	output.vertex_color = input.vert_color;
	output.view_ray = worldPos - cam_pos.xyz;
	output.extra = float4(0.0, 0.0, 0.0, 0.0);
	return output;
}

VS_OUTPUT passFullscreen(VS_INPUT input){ // covers the screen at the far plane and passes camera rays
	float2 ndc = float2((input.pos.x >= 0.0)? 1.0 : -1.0, (input.pos.y >= 0.0)? 1.0 : -1.0);
	if(abs(input.pos.x) < 0.0001 && abs(input.pos.y) < 0.0001) ndc = float2(0.0, 0.0); // center vertex of fan surfaces

	VS_OUTPUT output;
	output.pos = float4(ndc, 0.99999, 1.0);
	output.world_pos = cam_pos.xyz;
	output.world_normal = float3(0.0, 0.0, 0.0);
	output.texcoord = float3(ndc * 0.5 + 0.5, 0.0);
	output.vertex_color = input.vert_color;
	output.view_ray = fpsRay(ndc);
	output.extra = float4(ndc, 0.0, 0.0);
	return output;
}
