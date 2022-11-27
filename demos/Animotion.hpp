#include "Topl_App.hpp"

#include "Flat_Shader.hpp"

#include "primitives/Geo_Sphere.hpp"
#include "trees/Model.hpp"

#define MOTION_TIME 3.0

Input_CursorRange upRange = Input_CursorRange({ -1.0f, 1.0f }, { 0.8f, 1.0f });
Input_CursorRange leftRange = Input_CursorRange({ -0.8f, -1.0f }, { -1.0f, 1.0f });
Input_CursorRange downRange = Input_CursorRange({ -1.0f, 1.0f }, { -0.8f, -1.0f });
Input_CursorRange rightRange = Input_CursorRange({ 0.8f, 1.0f }, { -1.0f, 1.0f });

struct Animotion_App : public Topl_App {
    Animotion_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Animotion", backend){}

	void init() override;
	void loop(unsigned long frame) override;

	static Topl_Camera camera;
private:
	// Configurations

	Topl_Scene scene;

	Phys_Motion motion1 = Phys_Motion(MOTION_Instant, Vec3f({ 0.0f, 1.0f, 0.0f }), MOTION_TIME);
	Phys_Motion motion2 = Phys_Motion(MOTION_Linear, Vec3f({ -1.0f, 0.0f, 1.0f }), MOTION_TIME);
	Phys_Motion motion3 = Phys_Motion(MOTION_Orbit, Vec3f({ 0.5f, -5.0f, 1.0f }), Vec3f({ -1.0f, 10.0f, -1.0f }), MOTION_TIME);

	// Geometries and Drawable Objects

	Geo_Sphere sphere = Geo_Sphere({ 0.5f, 100, 100 });
	Geo_Actor sphereActor = Geo_Actor((Geo_Renderable*)&sphere);

	Geo_Model model1 = Geo_Model("SpinTop1", Topl_App::getModelsPath() + "SpinTop.obj");
	Geo_Model model2 = Geo_Model("SpinTop2", Topl_App::getModelsPath() + "SpinTop.obj");
	Geo_Model model3 = Geo_Model("SpinTop3", Topl_App::getModelsPath() + "SpinTop.obj");

	// Shaders and Pipelines

	Topl_Pipeline* pipeline;

	Flat_VertexShader vertShader;
	Flat_FragmentShader fragShader;
};