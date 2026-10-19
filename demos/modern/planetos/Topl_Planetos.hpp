#include "../Topl_Runtime.hpp"

// Planetos: stand on a small planet and walk all the way around it. The terrain keeps evolving, tides rise and fall,
// quakes send rings of rock across the surface and the sun circles the planet for day and night.
// The camera stays on top of the planet while the planet turns underneath it, so the usual FPS controls work anywhere

#define PLANETOS_RADIUS 80.0F
#define PLANETOS_FACE_DIVS 112 // grid divisions per cube face of the sphere mesh
#define PLANETOS_GEMS 24
#define PLANETOS_BIOMES 4

// Unit sphere made from a subdivided cube, spreads vertices more evenly than latitude rings

class Planetos_Sphere : public Geo_Mesh {
public:
	Planetos_Sphere(unsigned short divisions);
};

class Topl_Planetos : public Topl_Runtime {
public:
	Topl_Planetos(const char* execPath) : Topl_Runtime(execPath, "Planetos"){ }
	Topl_Planetos(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Planetos", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	void onPipelineSelect(unsigned index) override; // Pipelines menu: biomes first, then shading modes
	void printControls() const override;
private:
	Vec3f toWorld(const Vec3f& planetDir) const; // planet frame direction into world space
	Vec3f toPlanet(const Vec3f& worldDir) const; // world direction into the planet frame
	float getSurface(const Vec3f& planetDir) const; // terrain or sea, whichever is higher
	float getGround(float x, float z) const; // world height of the surface below a point near the top of the planet
	void travel(); // turns the planet under the camera by the distance walked this frame
	void orthonormalize();
	void updateSky();
	void placeGems();

	Vec3f _rows[3] = { Vec3f({ 1.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, 1.0F, 0.0F }), Vec3f({ 0.0F, 0.0F, 1.0F }) }; // planet frame to world
	Vec3f _center = Vec3f({ 0.0F, -PLANETOS_RADIUS, 0.0F });
	unsigned _biome = 0;
	float _heightScale = 5.5F;
	float _baseSea = 0.4F, _seaLevel = 0.4F;
	bool _isTides = true;
	float _terrainTime = 0.0F;
	unsigned _evolveSetting = 1;
	float _pulseAge = -1.0F; // seconds since the last quake, negative when none
	Vec3f _pulseOrigin = Vec3f({ 0.0F, 1.0F, 0.0F });
	float _dayAngle = 0.6F;
	unsigned _daySetting = 1;
	bool _isOrbit = false;

	Planet_VertexShader _planetVShader;
	Planet_PixelShader _planetPShader;
	Topl_Pipeline* _planetPipeline = nullptr;

	Planetos_Sphere _sphereMesh = Planetos_Sphere(PLANETOS_FACE_DIVS);
	Geo_Actor _planetActor = Geo_Actor("planetos_surface", &_sphereMesh);
	Topl_Scene _planetScene = Topl_Scene(&camera, { &_planetActor });

	// Floating gems scattered over the planet as landmarks
	Geo_Orb _gemMesh = Geo_Orb({ 1.0F, 6, 4 });
	Geo_Actor _gems[PLANETOS_GEMS];
	Vec3f _gemDirs[PLANETOS_GEMS]; // planet frame
	Topl_Scene _gemScene = Topl_Scene(&camera);
};
