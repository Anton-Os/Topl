#include "Subsurface.hpp"

// #define APP_BACKEND APP_OpenGL_4
#define APP_BACKEND APP_DirectX_11
// #define APP_BACKEND APP_Vulkan

unsigned radialImageCallback(double x, double y) {
	return blend(0xFF000000, 0xFFFFFFFF, sqrt(((x - 0.5) * (x - 0.5)) + ((y - 0.5) * (y - 0.5)))); // radial gradient
}

void Subsurface_App::init() {
	// Shaders and Pipeline

	if (APP_BACKEND == APP_OpenGL_4) {
		vertexShader = GL4_Textured_VertexShader();
		fragShader = GL4_Textured_FragmentShader();
	} else {
		vertexShader = Drx11_Textured_VertexShader();
		fragShader = Drx11_Textured_FragmentShader();
	}

	pipeline = Topl_Factory::genPipeline(APP_BACKEND, &vertexShader, &fragShader);

	// Geometries and Scene Elements

	std::string fontPath = std::string(Topl_App::getFontsPath() + "Tw-Cen-MT.ttf");
	Rasteron_Text text = Rasteron_Text({ fontPath.c_str(), "Topl", 0xFF000000, 0xFFFFFFFF });
	base.setTextImage(&text);

	material1.texUnits[MATERIAL_Albedo].setColorImage(0xFFFFFF00); // yellow
	material1.texUnits[MATERIAL_Height].setColorImage(0xFF0000FF); // red
	material1.texUnits[MATERIAL_Roughness].setColorImage(0xFF00FF00); // green
	material1.texUnits[MATERIAL_Opacity].setColorImage(0xFFFF0000); // blue
	material1.texUnits[MATERIAL_Enviornment].setColorImage(0xFF00FFFF); // cyan
	material1.texUnits[MATERIAL_Shadow].setColorImage(0xFFFF00FF); // magenta

	Rasteron_Image* testImage = createMappedImg({ volume1.getHeight(), volume1.getWidth() }, radialImageCallback);
	for (unsigned d = 0; d < volume1.getDepth(); d++) volume1.addSlice(testImage, d);
	deleteImg(testImage);

	scene.addGeometry("cube", &cubeActor);
	scene.addTexture("cube", base.getImage());
	scene.addMaterialTex("cube", &material1);
	scene.addVolumeTex("cube", &volume1);

	_renderer->buildScene(&scene);
}

void Subsurface_App::loop(double frameTime) {
	_renderer->setPipeline(pipeline);
	_renderer->updateScene(&scene);
	_renderer->renderScene(&scene);
}

int main(int argc, char** argv) {
	Subsurface_App app = Subsurface_App(argv[0], APP_BACKEND);

	app.run();
	return 0;
}