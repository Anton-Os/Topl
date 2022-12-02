#include "Topl_App.hpp"

#include "Effect_Shader.hpp"

#include "primitives/Geo_Flat.hpp"

Topl_Viewport viewport1 = Topl_Viewport(0, TOPL_WIN_HEIGHT / 2, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT / 2);
Topl_Viewport viewport2 = Topl_Viewport(0, 0, TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT / 2);
// Topl_Viewport viewport1 = Topl_Viewport(TOPL_WIN_WIDTH / 2, 0, TOPL_WIN_WIDTH / 2, TOPL_WIN_HEIGHT);
// Topl_Viewport viewport2 = Topl_Viewport(0, 0, TOPL_WIN_WIDTH / 2, TOPL_WIN_HEIGHT);

struct Splitscreen_App : public Topl_App {
    Splitscreen_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Splitscreen", backend){}

	void init() override;
	void loop(double frameTime) override;
private:
	// Configurations

	Topl_Scene scene;

	// Geometries and Drawable Objects

	Geo_FlatSquare plane = Geo_FlatSquare(1000.0);
	Geo_Actor planeActor = Geo_Actor((Geo_Renderable*)&plane);

	// Shaders and Pipelines

	Topl_Pipeline *pipeline1, *pipeline2;

	Effect_VertexShader vertexShader1, vertexShader2;
	Effect_FragmentShader fragShader;
};