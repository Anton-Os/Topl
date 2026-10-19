#include "../Topl_Runtime.hpp"

// Fractile: fly through raymarched 3D fractals, the mandelbulb, menger sponge, sierpinski tetrahedron,
// mandelbox and a quaternion julia set, each morphing over time and shaded with orbit trap colors

struct Fractile_Preset {
	const char* name;
	float shape; // bulb power or box scale
	unsigned iterations;
	float worldScale; // fractal units to world units
	float bound; // bounding radius in fractal units
	float glow;
	Vec3f start; // camera start position
};

class Topl_Fractile : public Topl_Runtime {
public:
	Topl_Fractile(const char* execPath) : Topl_Runtime(execPath, "Fractile"){ }
	Topl_Fractile(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Fractile", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	void onPipelineSelect(unsigned index) override; // Pipelines menu: fractals first, then shading modes
	bool isPickEnabled() const override { return false; } // nothing to pick inside the fractal
	void printControls() const override;
private:
	void selectFractal(unsigned index);
	float getSurfaceDistance() const; // rough distance to the fractal surface for the camera speed

	FRACTAL_Type _fractal = FRACTAL_Mandelbulb;
	unsigned _iterations = 8;
	int _detailLevel = 1; // 0 coarse, 1 normal, 2 fine
	bool _isMorphing = true;
	bool _isSpinning = false;
	float _spin = 0.0F;
	float _colorShift = 0.0F;
	double _morphTime = 0.0;

	Fractal_VertexShader _fractalVShader;
	Fractal_PixelShader _fractalPShader;
	Topl_Pipeline* _fractalPipeline = nullptr;
};
