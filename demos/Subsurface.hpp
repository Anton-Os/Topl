#include "Topl_App.hpp"

#include "Textured_Shader.hpp"

#include "primitives/Geo_Extruded.hpp"

struct Subsurface_App : public Topl_App {
	Subsurface_App(const char* execPath, APP_Backend backend)
		: Topl_App(execPath, "Subsurface", backend) {}

	void init() override;
	void loop(double frameTime) override;
private:
	// Configurations

	Topl_Scene scene;

	// Geometries and Drawable Objects

	Geo_ExtrudedSquare cube = Geo_ExtrudedSquare();
	Geo_Actor cubeActor = Geo_Actor((Geo_Renderable*)&cube);

	Img_Base base;
	Img_Material material1;
	Img_Volume volume1 = Img_Volume(64);
	Img_Heightmap heightmap1;

	// Shaders and Pipelines;

	Topl_Pipeline* pipeline;

	Textured_VertexShader vertexShader;
	Textured_FragmentShader fragShader;
};