// Modern Vertex: vertex inputs, actor transforms and the FPS camera

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec3 texcoord_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec3 vert_color_in;
layout(location = 4) in vec3 tangent_in;

// FPS Camera
// Yaw turns around the world up axis, pitch tilts the view, roll banks it. Yaw 0 looks down +Z

float getCamZoom(){ return (cam_pos.w > 0.0)? cam_pos.w : 1.0; }
float getCamFov(){ return (cam_lens.x > 0.0)? cam_lens.x : 1.0472; }
float getCamAspect(){ return (cam_lens.y > 0.0)? cam_lens.y : 1.0; }
float getCamNear(){ return (cam_lens.z > 0.0)? cam_lens.z : 0.05; }
float getCamFar(){ return (cam_lens.w > getCamNear())? cam_lens.w : 400.0; }
float getCamFocal(){ return getCamZoom() / tan(getCamFov() * 0.5); }

void fpsBasis(out vec3 right, out vec3 up, out vec3 forward){
	float yaw = look_pos.x;
	float pitch = clamp(look_pos.y, -1.55, 1.55);
	float roll = look_pos.z;

	forward = vec3(sin(yaw) * cos(pitch), sin(pitch), cos(yaw) * cos(pitch));
	vec3 flatRight = vec3(cos(yaw), 0.0, -sin(yaw));
	vec3 flatUp = cross(forward, flatRight);

	right = flatRight * cos(roll) + flatUp * sin(roll);
	up = flatUp * cos(roll) - flatRight * sin(roll);
}

vec3 fpsView(vec3 worldPos){ // world space into camera space (x right, y up, z forward)
	vec3 right, up, forward;
	fpsBasis(right, up, forward);
	vec3 rel = worldPos - cam_pos.xyz;
	return vec3(dot(rel, right), dot(rel, up), dot(rel, forward));
}

vec4 fpsProject(vec3 viewPos){ // perspective projection, depth maps from 0 at the near plane to 1 at the far plane
	float n = getCamNear(); // the 0 to 1 range keeps menu overlays drawn at depth 0 in front of the world, like DirectX
	float f = getCamFar();
	float focal = getCamFocal();
	return vec4(
		viewPos.x * focal / getCamAspect(),
		viewPos.y * focal,
		viewPos.z * (f / (f - n)) - ((f * n) / (f - n)),
		viewPos.z
	);
}

vec4 fpsTransform(vec3 worldPos){ return fpsProject(fpsView(worldPos)); }

vec3 fpsRay(vec2 ndc){ // world space ray through a point on screen
	vec3 right, up, forward;
	fpsBasis(right, up, forward);
	float focal = getCamFocal();
	return forward + right * (ndc.x * getCamAspect() / focal) + up * (ndc.y / focal);
}

// Actor Transforms

vec3 getWorldPos(vec3 localPos){ return actorRotate(localPos * scale, rotation) + offset; }

vec3 getWorldNormal(vec3 localNormal){
	vec3 s = max(abs(scale), vec3(0.0001));
	return actorRotate(localNormal / s, rotation);
}

// Engine meshes do not carry reliable normals, so modern shaders use facet normals from derivatives in the
// fragment stage. Actors flagged with actor_params.w > 0.5 are treated as spheres and get smooth radial normals
vec3 getModelNormal(vec3 localPos){
	if(actor_params.w <= 0.5 || dot(localPos, localPos) < 1e-10) return vec3(0.0); // zero selects facet normals
	return getWorldNormal(normalize(localPos));
}

void passVaryings(vec3 worldPos, vec3 worldNormal){
	world_pos = worldPos;
	world_normal = worldNormal;
	texcoord = texcoord_in;
	vertex_color = vert_color_in;
	view_ray = worldPos - cam_pos.xyz;
	extra = vec4(0.0);
}

void passFullscreen(){ // covers the screen at the far plane and passes camera rays
	vec2 ndc = vec2((pos_in.x >= 0.0)? 1.0 : -1.0, (pos_in.y >= 0.0)? 1.0 : -1.0);
	if(abs(pos_in.x) < 0.0001 && abs(pos_in.y) < 0.0001) ndc = vec2(0.0); // center vertex of fan surfaces

	gl_Position = vec4(ndc, 0.99999, 1.0);
	world_pos = cam_pos.xyz;
	world_normal = vec3(0.0);
	texcoord = vec3(ndc * 0.5 + 0.5, 0.0);
	vertex_color = vert_color_in;
	view_ray = fpsRay(ndc);
	extra = vec4(ndc, 0.0, 0.0);
}
