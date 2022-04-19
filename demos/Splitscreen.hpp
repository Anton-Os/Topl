#include "Topl_App.hpp"

#include "Flat_Shader.hpp"

struct Splitscreen_App : public Topl_App {
    Splitscreen_App(const char* execPath, APP_Backend backend, std::initializer_list<Topl_Viewport> viewports) 
		: Topl_App(execPath, "Splitscreen", backend, viewports){}

	void init() override;
	void loop(unsigned long frame) override;
private:
	// Configurations
	// Geometries and Drawable Objects
	// Shaders and Pipelines

	Topl_Pipeline* pipeline;

	Flat_VertexShader vertShader;
	Flat_FragmentShader fragShader;
};