// Modern Demo Implemented over Topl_App class

#include "Topl_App.hpp"

#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Layered_Shader.hpp"
#include "Advance_Shader.hpp"

#include "primitives/Geo_Sphere.hpp"
#include "trees/Grid.hpp"
#include "trees/Pane.hpp"

struct Playground_App : public Topl_App {
    Playground_App(const char* execPath, APP_Backend b) 
		: Topl_App(execPath, "Playground", b){}

	void init() override;
	void loop(unsigned long frame) override;

	// Configurations

	static Topl_Camera camera1;
	static Topl_Camera camera2;
	static Phys_Motion inOutMotion;
	Topl_Scene scene_main, scene_overlay, scene_details;

private:
	void postFrame();

	// Geometries and Drawable Objects
	Geo_Sphere sphere = Geo_Sphere({ 0.5f, 6, 6 });
	Geo_Actor sphereActor = Geo_Actor((Geo_RenderObj*)&sphere);

	// Geo_Grid grid = Geo_Grid("grid", &squareActor, Geo_Grid_Properties(std::make_pair(80, 0.25f)));
	Geo_FlatSquare captureSquare = Geo_FlatSquare(0.5f);
	Geo_Actor captureActor = Geo_Actor((Geo_RenderObj*)&captureSquare);
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 5);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 2);

	Rasteron_FormatText textObj = { "", "", WHITE_COLOR, BLACK_COLOR};
	Topl_Image heightmapImage = Topl_Image(imagesPath + "FlowerIllusion.png");
	Topl_Heightmap heightmap = Topl_Heightmap(heightmapImage.getImage());
	Topl_Material material = Topl_Material("material1", 1024, 1024);

	// Shaders and Pipelines

	Topl_Pipeline* colorPipeline;
	Topl_Pipeline* texPipeline;
	// Topl_Pipeline* advancePipeline;

	Textured_VertexShader vertexShader1;
	Textured_FragmentShader fragShader1;
	Flat_VertexShader vertexShader2;
	Flat_FragmentShader fragShader2;
	Advance_TessCtrlShader tessCtrlShader;
	Advance_TessEvalShader tessEvalShader;
	Advance_GeometryShader geomShader;
};