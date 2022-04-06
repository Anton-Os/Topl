// Modern Demo Implemented over Topl_App class

#include "Topl_App.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Layered_Shader.hpp"
#include "Advance_Shader.hpp"

#include "trees/Grid.hpp"
#include "trees/Pane.hpp"

struct Playground_App : public Topl_App {
    Playground_App(const char* execPath, APP_Backend b) 
		: Topl_App(execPath, "Playground", b){}

	void init() override;
	void loop(unsigned long frame) override;

	// Configurations

	Topl_Camera camera_main;
	Topl_Camera camera_back;
	Topl_Scene scene_main; // for graphics
	Topl_Scene scene_overlay; // for interaction
	Topl_Scene scene_props; // for details

	// Geometries and Drawable Objects

	Geo_FlatSquare square = Geo_FlatSquare(0.25f);
	Geo_Actor squareActor = Geo_Actor((Geo_RenderObj*)&square);
	Geo_DuplexShape duplexShape = Geo_DuplexShape((Geo_RenderObj*)&square, 3);
	Geo_TessShape tessShape = Geo_TessShape((Geo_RenderObj*)&square, 3);
	Geo_Grid grid = Geo_Grid("grid", &squareActor, Geo_Grid_Properties(std::make_pair(80, 0.25f)));
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 5);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 2);

	Topl_Image heightmapImage = Topl_Image(imagesPath + "FlowerIllusion.png");
	Topl_Heightmap heightmap = Topl_Heightmap(heightmapImage.getImage());
	Topl_Material material = Topl_Material("material1", 1024, 1024);

	// Shaders and Pipelines

	Topl_Pipeline* flatPipeline;
	// Topl_Pipeline* advancePipeline;

	Flat_VertexShader vertexShader;
	Flat_FragmentShader fragShader;
	Advance_TessCtrlShader tessCtrlShader;
	Advance_TessEvalShader tessEvalShader;
	Advance_GeometryShader geomShader;
};