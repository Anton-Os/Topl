#include "../Topl_Runtime.hpp"

// Torspin: the camera rides inside the tube of a giant torus that spins around its axis while that axis keeps tilting,
// so the walls stream past and the tunnel ahead bends, rolls and climbs. Glowing lanterns drift with the walls.
// The torus is raymarched by the tunnel pipeline and the lanterns are regular meshes placed with the same transform

#define TORSPIN_LANTERNS 12
#define TORSPIN_SHAPES 3

struct Torspin_Shape { float major, tube, ribs, ribDepth, pipes, panels; };

class Topl_Torspin : public Topl_Runtime {
public:
	Topl_Torspin(const char* execPath) : Topl_Runtime(execPath, "Torspin"){ }
	Topl_Torspin(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Torspin", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	void printControls() const override;
private:
	Vec3f toTorus(const Vec3f& world) const; // world space into the spinning torus frame
	Vec3f toWorld(const Vec3f& local) const; // torus frame back into world space
	void updateTilt();
	void recenter(); // folds camera travel along the tube into the spin so the camera stays near the anchor
	void placeLanterns(float t);

	const Torspin_Shape& getShape() const;

	unsigned _shape = 0;
	float _spin = 0.0F, _spinSpeed = 0.3F; // radians per second
	float _tiltTime = 0.0F, _tiltSpeed = 1.0F;
	bool _isTilting = true;
	float _hueShift = 0.0F;
	Vec3f _tilt[3] = { Vec3f({ 1.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, 1.0F, 0.0F }), Vec3f({ 0.0F, 0.0F, 1.0F }) }; // columns, torus frame to world
	Vec3f _anchor = Vec3f({ 0.0F, 0.0F, 0.0F }); // point on the centerline the torus turns around

	Tunnel_VertexShader _tunnelVShader;
	Tunnel_PixelShader _tunnelPShader;
	Topl_Pipeline* _tunnelPipeline = nullptr;

	// Lanterns
	Geo_Orb _lanternMesh = Geo_Orb({ 1.0F, 24, 24 });
	Geo_Actor _lanterns[TORSPIN_LANTERNS];
	float _lanternAngles[TORSPIN_LANTERNS]; // position along the torus in the torus frame
	float _lanternPhases[TORSPIN_LANTERNS];
	bool _isLanterns = true;
	Topl_Scene _lanternScene = Topl_Scene(&camera);
};
