#include "../Topl_Runtime.hpp"

#include "constructs/Geo_Puppet.hpp"

// Ragdolls: puppets from Geo_Puppet held together by the spring connectors in support/physics.
// 3D puppets are built from engine meshes and tumble around an arena, a marionette hangs from a swinging anchor,
// and the sprite puppets of the legacy Animatrix demo flop around on a flat 2D lane behind them

#define RAGDOLL_PUPPETS 4 // free 3D puppets
#define RAGDOLL_SPRITES 3 // 2D sprite puppets, available with textures
#define RAGDOLL_ARENA 13.0F // half width of the arena
#define RAGDOLL_LANE_Z 9.0F // depth of the 2D lane
#define RAGDOLL_GRAVITY 9.8F

enum RAGDOLL_Stiffness { RAGDOLL_Loose = 0, RAGDOLL_Normal = 1, RAGDOLL_Rigid = 2 };

struct Ragdoll_Body { // a puppet with the values the simulation needs around it
	Geo_Puppet* puppet = nullptr;
	float radii[PUPPET_PARTS]; // collision radius of each part
	Vec3f pose[PUPPET_PARTS]; // standing pose relative to the body
	bool isFlat = false; // locked to the 2D lane
	bool isBlocks = false; // built from meshes, limbs are turned to follow the joints
	Vec3f spawn;
};

class Topl_Ragdolls : public Topl_Runtime {
public:
	Topl_Ragdolls(const char* execPath) : Topl_Runtime(execPath, "Ragdolls"){ }
	Topl_Ragdolls(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Ragdolls", backend){ }
	~Topl_Ragdolls();
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	void printControls() const override;
private:
	void buildBlockPuppet(unsigned index, Vec3f spawn, Modern_Float4 color);
#ifdef TOPL_ENABLE_TEXTURES
	void buildSpritePuppet(unsigned index, Vec3f spawn, const std::string& character);
#endif
	void placePose(Ragdoll_Body* body, Vec3f origin, float tilt); // moves every part to the standing pose
	void drop(); // lifts every puppet back above the floor
	void setStiffness(RAGDOLL_Stiffness stiffness);
	void stepPhysics(double frameSecs);
	void collide(Ragdoll_Body* body);
	void orient(Ragdoll_Body* body); // turns limbs along the joint between them and the body
	Ragdoll_Body* findBody(const Geo_Actor* actor, unsigned* part);

	std::vector<Ragdoll_Body> _bodies;
	Topl_Scene _ragdollScene = Topl_Scene(&camera);
	Timer_Persist _physicsTimer;

	float _gravityScale = 1.0F;
	bool _isWind = false, _isGrab = false, _isFrozen = false, _isSwinging = true, _isLane = true;
	RAGDOLL_Stiffness _stiffness = RAGDOLL_Normal;

	// Marionette
	Vec3f _anchorPoint = Vec3f({ 0.0F, 6.0F, 4.0F });
	Phys_Connector _anchorLink;
	Geo_Actor _anchorActor;
	Topl_Scene _propScene = Topl_Scene(&camera);

	// Arena
	Runtime_Grid _floorMesh = Runtime_Grid(RAGDOLL_ARENA * 2.0F + 8.0F, 48);
	Geo_Actor _floorActor = Geo_Actor("ragdoll_floor", &_floorMesh);
	Topl_Scene _floorScene = Topl_Scene(&camera, { &_floorActor });
};
