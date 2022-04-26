#include "Topl_App.hpp"

#include "Flat_Shader.hpp"

#include "trees/Model.hpp"

#define MOTION_TIME 3.0

struct Animotion_App : public Topl_App {
    Animotion_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Animotion", backend){}

	void init() override;
	void loop(unsigned long frame) override;
private:
	// Configurations

	Topl_Scene scene;
	Topl_Camera camera;

	Phys_Motion motion1 = Phys_Motion(MOTION_Linear, Vec3f({ 0.0f, 1.0f, 0.0f }), MOTION_TIME);
	Phys_Motion motion2 = Phys_Motion(MOTION_Linear, Vec3f({ -1.0f, 0.0f, 1.0f }), MOTION_TIME);
	Phys_Motion motion3 = Phys_Motion(MOTION_Linear, Vec3f({ 0.5f, -5.0f, 1.0f }), Vec3f({ -1.0f, 10.0f, -1.0f }), MOTION_TIME);

	// Geometries and Drawable Objects

	Geo_Model model = Geo_Model("SpinTop", modelsPath + "SpinTop.obj"); // model object

	// Shaders and Pipelines

	Topl_Pipeline* pipeline;

	Flat_VertexShader vertShader;
	Flat_FragmentShader fragShader;
};