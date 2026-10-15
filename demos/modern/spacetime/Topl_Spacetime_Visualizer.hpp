#include "../Topl_Runtime.hpp"

// Spacetime Visualizer: planets orbit a star while the fabric of spacetime bends beneath them

#define SPACETIME_PLANETS 3
#define SPACETIME_FABRIC_SIZE 80.0F
#define SPACETIME_FABRIC_DIVS 200
#define SPACETIME_STAR_MASS 3.0F

struct Spacetime_Body {
	float mass;
	float orbitRadius;
	float orbitSpeed; // radians per second
	float phase;
	float size;
};

class Topl_Spacetime_Visualizer : public Topl_Runtime {
public:
	Topl_Spacetime_Visualizer(const char* execPath) : Topl_Runtime(execPath, "Spacetime"){ }
	Topl_Spacetime_Visualizer(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Spacetime", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	void printControls() const override;
private:
	void updateBodies();

	float _gravity = 1.0F;
	bool _isBinary = false; // splits the star into an orbiting pair
	bool _isWaves = true;
	unsigned _visiblePlanets = SPACETIME_PLANETS;

	Spacetime_Body _bodies[SPACETIME_PLANETS + 1] = {
		{ SPACETIME_STAR_MASS, 0.0F, 0.0F, 0.0F, 2.2F }, // star
		{ 0.6F, 8.0F, 0.0F, 0.0F, 0.7F },
		{ 1.0F, 14.0F, 0.0F, 2.1F, 1.0F },
		{ 0.8F, 21.0F, 0.0F, 4.2F, 0.85F },
	};

	Runtime_Grid _fabricMesh = Runtime_Grid(SPACETIME_FABRIC_SIZE, SPACETIME_FABRIC_DIVS);
	Geo_Actor _fabricActor = Geo_Actor("spacetime_fabric", &_fabricMesh);
	Topl_Scene _fabricScene = Topl_Scene(&camera, { &_fabricActor });

	Geo_Orb _bodyMesh = Geo_Orb({ 1.0F, 32, 32 });
	Geo_Actor _bodyActors[SPACETIME_PLANETS + 1] = {
		Geo_Actor("star", &_bodyMesh), Geo_Actor("planet_1", &_bodyMesh), Geo_Actor("planet_2", &_bodyMesh), Geo_Actor("planet_3", &_bodyMesh)
	};
	Topl_Scene _bodyScene = Topl_Scene(&camera, { &_bodyActors[0], &_bodyActors[1], &_bodyActors[2], &_bodyActors[3] });
};
