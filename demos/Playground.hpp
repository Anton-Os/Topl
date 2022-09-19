#include "Topl_App.hpp"

#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Layered_Shader.hpp"
#include "Advance_Shader.hpp"

#include "Primitives/Geo_Conic.hpp"
#include "primitives/Geo_Sphere.hpp"
#include "trees/Grid.hpp"
#include "trees/Pane.hpp"
#include "trees/Humanoid.hpp"

struct Playground_App : public Topl_App {
    Playground_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Playground", backend){}

	void init() override;
	void loop(unsigned long frame) override;

	// Configurations

	static Topl_Camera camera1, camera2, camera3;
	Topl_Scene scene_main, scene_pawns, scene_overlay, scene_details;
	Phys_Connector anchor1, anchor2, anchor3, anchor4;

private:
	void preFrame();
	void postFrame();

	void genShaderPipeline();

	// Geometries and Drawable Objects
	Geo_Sphere sphere = Geo_Sphere({ 0.5f, 200, 200 });
	// Geo_Sphere sphere = Geo_Sphere({ 0.5f, 12, 6 });
	Geo_Actor sphereActor = Geo_Actor((Geo_RenderObj*)&sphere);
	Geo_ConicTriangle cone1 = Geo_ConicTriangle(0.1f); Geo_ConicHex cone2 = Geo_ConicHex(0.1f, Vec3f({ 0.1f, 0.5f, -0.2f }));
	Geo_Actor coneActor1 = Geo_Actor((Geo_RenderObj*)&cone1); Geo_Actor coneActor2 = Geo_Actor((Geo_RenderObj*)&cone2);
	Geo_Grid_Params gridParams = Geo_Grid_Params(std::make_pair(10, 0.25f), std::make_pair(1, 0.0f), std::make_pair(10, 0.1f));
	Geo_Grid grid = Geo_Grid("grid", &coneActor1, gridParams);
	// Geo_Humanoid2D demon, angel, ghost;
	// Geo_Humanoid3D avatar;
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 9);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 3);

	Rasteron_FormatText textObj = { "option", "", WHITE_COLOR, BLACK_COLOR};
	// Topl_Image heightmapImage = Topl_Image(imagesPath + "FlowerIllusion.png");
	// Topl_Heightmap heightmap = Topl_Heightmap(heightmapImage.getImage());
	Topl_Material material = Topl_Material("material1", 1024, 1024);

	// Shaders and Pipelines

	Topl_Pipeline *colPipeline, *texPipeline, *litPipeline;
	// Topl_Pipeline* advancePipeline;

	Textured_VertexShader vertexShader1;
	Textured_FragmentShader fragShader1;
	Flat_VertexShader vertexShader2;
	Flat_FragmentShader fragShader2;
	Beams_VertexShader vertexShader3;
	Beams_FragmentShader fragShader3;
	Layered_VertexShader vertexShader4;
	Layered_FragmentShader fragShader4;
	Advance_TessCtrlShader tessCtrlShader;
	Advance_TessEvalShader tessEvalShader;
	Advance_GeometryShader geomShader;
};