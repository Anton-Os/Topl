#include "Modern_Common.hlsl"

#include "Modern_Vertex.hlsl"

#include "Planet.hlsl"

// Planet surface: every vertex of a unit sphere is pushed out by the terrain, oceans fill everything below sea level

float3 getSurfacePoint(float3 dir){ return dir * (getRadius() + max(planetTerrain(dir), getSeaLevel())); }

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID) {
	float3 dir = normalize(input.pos.xyz);
	float h = planetTerrain(dir);
	float sea = getSeaLevel();
	float3 local = dir * (getRadius() + max(h, sea));

	float3 t1 = planetTangent(dir); // smooth normal from neighbouring terrain samples
	float3 t2 = cross(dir, t1);
	float e = 0.0025;
	float3 p1 = getSurfacePoint(normalize(dir + t1 * e));
	float3 p2 = getSurfacePoint(normalize(dir + t2 * e));
	float3 normal = normalize(cross(p1 - local, p2 - local));
	if(dot(normal, dir) < 0.0) normal = -normal;

	float3 worldPos = params[5].xyz + planetToWorld(local);
	VS_OUTPUT output = passVaryings(input, worldPos, planetToWorld(normal));
	output.texcoord = dir; // planet frame direction
	output.extra = float4(h, sea, fpsView(worldPos).z, 0.0);
	return output;
}
