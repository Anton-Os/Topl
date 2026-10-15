#include "../Topl_Runtime.hpp"

// Visualizer: an endless procedural terrain with a gallery of lit, toon and hologram objects plus a raymarched world

#define VISUALIZER_OBJECTS 9
#define VISUALIZER_RING 9.0F // radius of the gallery ring
#define VISUALIZER_TERRAIN_SIZE 140.0F
#define VISUALIZER_TERRAIN_DIVS 180

enum VISUALIZER_View {
	VISUALIZER_Gallery = 0, // every object group uses its own pipeline
	VISUALIZER_Lighting = 1,
	VISUALIZER_Toon = 2,
	VISUALIZER_Hologram = 3,
	VISUALIZER_Raymarch = 4
};

class Topl_Visualizer : public Topl_Runtime {
public:
	Topl_Visualizer(const char* execPath) : Topl_Runtime(execPath, "Visualizer"){ }
	Topl_Visualizer(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Visualizer", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	void onPipelineSelect(unsigned index) override; // Pipelines menu: views first, then terrain modes
	bool isPickEnabled() const override { return _view != VISUALIZER_Raymarch; }
	void printControls() const override;
private:
	void arrangeGallery();
	MODERN_Pipeline getGroupPipeline(unsigned group) const;

	VISUALIZER_View _view = VISUALIZER_Gallery;
	int _terrainMode = 0;

	// Terrain follows the camera in grid sized steps so it never runs out
	Runtime_Grid _terrainMesh = Runtime_Grid(VISUALIZER_TERRAIN_SIZE, VISUALIZER_TERRAIN_DIVS);
	Geo_Actor _terrainActor = Geo_Actor("visualizer_terrain", &_terrainMesh);
	Topl_Scene _terrainScene = Topl_Scene(&camera, { &_terrainActor });

	// Gallery meshes, three per pipeline group
	Geo_Orb _orbMesh = Geo_Orb({ 1.0F, 32, 32 });
	Geo_Quad3D _cubeMesh = Geo_Quad3D(1.0F);
	Geo_Torus _torusMesh = Geo_Torus(0.5F);
	Geo_Hex3D _hexMesh = Geo_Hex3D(1.0F);
	Geo_Circle3D _cylinderMesh = Geo_Circle3D(0.8F);
	Geo_Trig3D _prismMesh = Geo_Trig3D(1.0F);

	Geo_Actor _objects[VISUALIZER_OBJECTS] = {
		Geo_Actor("lit_orb", &_orbMesh), Geo_Actor("lit_cube", &_cubeMesh), Geo_Actor("lit_torus", &_torusMesh),
		Geo_Actor("toon_orb", &_orbMesh), Geo_Actor("toon_hex", &_hexMesh), Geo_Actor("toon_prism", &_prismMesh),
		Geo_Actor("holo_orb", &_orbMesh), Geo_Actor("holo_cylinder", &_cylinderMesh), Geo_Actor("holo_torus", &_torusMesh)
	};
	Topl_Scene _groupScenes[3] = {
		Topl_Scene(&camera, { &_objects[0], &_objects[1], &_objects[2] }),
		Topl_Scene(&camera, { &_objects[3], &_objects[4], &_objects[5] }),
		Topl_Scene(&camera, { &_objects[6], &_objects[7], &_objects[8] })
	};
	Vec3f _basePositions[VISUALIZER_OBJECTS];
};
