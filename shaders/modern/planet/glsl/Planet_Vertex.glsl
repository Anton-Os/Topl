#version 440

#define STAGE_VERTEX

#include "Modern_Common.glsl"

#include "Modern_Vertex.glsl"

#include "Planet.glsl"

// Planet surface: every vertex of a unit sphere is pushed out by the terrain, oceans fill everything below sea level

vec3 getSurfacePoint(vec3 dir){ return dir * (getRadius() + max(planetTerrain(dir), getSeaLevel())); }

void main() {
	vec3 dir = normalize(pos_in);
	float h = planetTerrain(dir);
	float sea = getSeaLevel();
	vec3 local = dir * (getRadius() + max(h, sea));

	vec3 t1 = planetTangent(dir); // smooth normal from neighbouring terrain samples
	vec3 t2 = cross(dir, t1);
	float e = 0.0025;
	vec3 p1 = getSurfacePoint(normalize(dir + t1 * e));
	vec3 p2 = getSurfacePoint(normalize(dir + t2 * e));
	vec3 normal = normalize(cross(p1 - local, p2 - local));
	if(dot(normal, dir) < 0.0) normal = -normal;

	vec3 worldPos = params[5].xyz + planetToWorld(local);
	gl_Position = fpsTransform(worldPos);
	passVaryings(worldPos, planetToWorld(normal));
	texcoord = dir; // planet frame direction
	extra = vec4(h, sea, fpsView(worldPos).z, 0.0);
}
