#include "Topl_App.hpp"

#include "Effect_Shader.hpp"

#include "primitives/Geo_Flat.hpp"

struct Splitscreen_App : public Topl_App {
    Splitscreen_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Splitscreen", backend){}

	void init() override;
	void loop(unsigned long frame) override;
private:
	// Configurations

	Topl_Scene scene;
	Topl_Viewport viewports[2] = {
		Topl_Viewport(0, TOPL_WIN_HEIGHT / 2, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT / 2), // bottom-half pane
		Topl_Viewport(0, 0, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT / 2), // top-half pane
	};

	// Geometries and Drawable Objects

	Geo_Plane plane;
	Geo_Actor planeActor = Geo_Actor((Geo_RenderObj*)&plane);

	// Shaders and Pipelines

	Topl_Pipeline* pipeline;

	Effect_VertexShader vertexShader1, vertexShader2, vertexShader3, vertexShader4;
	Effect_FragmentShader fragShader;
};