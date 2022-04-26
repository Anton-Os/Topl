#include "Topl_App.hpp"

#include "Effect_Shader.hpp"

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

	Effect_VertexShader vertexShader1, vertexShader2, vertexShader3, vertexShader4;
	Effect_FragmentShader fragShader;
};