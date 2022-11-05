#include "Topl_App.hpp"

#include "Idle_Shader.hpp"
#include "Advance_Shader.hpp"

#define APP_BACKEND APP_OpenGL_4
// #define APP_BACKEND APP_DirectX_11
// #defint APP_BACKEND App_Vulkan

struct Diagnostic_App : public Topl_App {
	Diagnostic_App(const char* execPath, APP_Backend backend)
		: Topl_App(execPath, "Diagnostic", backend) {}

	void init() override { /* empty */ };
	void loop(unsigned long frame) override { /* empty */ };

	Idle_VertexShader vertexShader;
	Idle_FragmentShader fragShader;
};

int main(int argc, char** argv) {
	{
		Diagnostic_App test = Diagnostic_App(argv[0], APP_BACKEND);
	}

	Diagnostic_App app = Diagnostic_App(argv[0], APP_BACKEND);
	app.run(); 

	return 0;
}