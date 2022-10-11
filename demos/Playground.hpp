#include "Topl_App.hpp"

#include "Textured_Shader.hpp"
#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Advance_Shader.hpp"

#include "Primitives/Geo_Conic.hpp"
#include "primitives/Geo_Sphere.hpp"
#include "trees/Geo_Sequence.hpp"
#include "trees/Geo_Grid.hpp"
#include "trees/Pane.hpp"
#include "trees/Humanoid.hpp"

#define PLAYGROUND_SPHERES_COUNT 6
#define PLAYGROUND_PANE_COUNT 9

#define HUMANOID_SCALE 0.25f

std::string ghostAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::imagesPath + "Ghost-Head.png",
	Topl_App::imagesPath + "Ghost-LeftArm.png",
	Topl_App::imagesPath + "Ghost-RightArm.png",
	Topl_App::imagesPath + "Ghost-Torso.png",
	Topl_App::imagesPath + "Ghost-LeftLeg.png",
	Topl_App::imagesPath + "Ghost-RightLeg.png"
};

std::string angelAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::imagesPath + "Angel-Head.png",
	Topl_App::imagesPath + "Angel-LeftWing.png",
	Topl_App::imagesPath + "Angel-RightWing.png",
	Topl_App::imagesPath + "Angel-Torso.png",
	Topl_App::imagesPath + "Angel-LeftLeg.png",
	Topl_App::imagesPath + "Angel-RightLeg.png"
};

std::string demonAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::imagesPath + "Demon-Head.png",
	Topl_App::imagesPath + "Demon-LeftWing.png",
	Topl_App::imagesPath + "Demon-RightWing.png",
	Topl_App::imagesPath + "Demon-Torso.png",
	Topl_App::imagesPath + "Demon-LeftLeg.png",
	Topl_App::imagesPath + "Demon-RightLeg.png"
};

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
	void createScene_Main();
	void createScene_Overlay();
	void createScene_Details();

	// Main Scene Elements
	Geo_FlatSquare square = Geo_FlatSquare(1.0f);
	Geo_Actor platformActor = Geo_Actor((Geo_RenderObj*)&square);
	Geo_Sphere sphere = Geo_Sphere({ 0.025f, 200, 200 });
	Geo_Actor sphereActors[PLAYGROUND_SPHERES_COUNT];
	Geo_Humanoid2D ghost = Geo_Humanoid2D("ghost", ghostAssets);
	Geo_Humanoid2D angel = Geo_Humanoid2D("angel", angelAssets);
	Geo_Humanoid2D demon = Geo_Humanoid2D("demon", demonAssets);
	// Geo_Humanoid2D demon, angel, ghost;
	// Geo_Humanoid3D avatar;

	// Overlay Scene Elements
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 9);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 3);

	// Detail Scene Elements
	Geo_ConicTriangle cone1 = Geo_ConicTriangle(0.2f); 
	Geo_ConicHex cone2 = Geo_ConicHex(0.2f, Vec3f({ 0.1f, 0.5f, -0.2f }));
	Geo_Actor coneActor1 = Geo_Actor((Geo_RenderObj*)&cone1);
	Geo_Actor coneActor2 = Geo_Actor((Geo_RenderObj*)&cone2);
	// Geo_Grid_Params gridParams = Geo_Grid_Params(std::make_pair(10, 0.25f), std::make_pair(1, 0.0f), std::make_pair(10, 0.1f));
	// Geo_Grid grid = Geo_Grid("grid", &coneActor1, gridParams);

	// Textures, Materials, Heightmaps

	Rasteron_Image* frameImage = nullptr;
	std::string fontStr = std::string(fontsPath + "MajorMonoDisplay-Regular.ttf");
	// Rasteron_Text textObj = { fontStr.c_str(), "option", WHITE_COLOR, BLACK_COLOR};
	Rasteron_Text boxedTextObjs[PLAYGROUND_PANE_COUNT];
	Topl_Image boxedPaneImages[PLAYGROUND_PANE_COUNT];
	Topl_Material material = Topl_Material("material1", 1024, 1024);
	Topl_Image heightmapImage = Topl_Image(imagesPath + "BigGrid.png");
	Topl_Heightmap heightmap = Topl_Heightmap(heightmapImage.getImage());
	Geo_Actor heightmapActor = Geo_Actor((Geo_RenderObj*)&heightmap);

	// Shaders and Pipelines

	Topl_Pipeline *colPipeline, *texPipeline, *litPipeline;
	Topl_Pipeline* advancePipeline;

	Textured_VertexShader vertexShader1; Textured_FragmentShader fragShader1;
	Flat_VertexShader vertexShader2; Flat_FragmentShader fragShader2;
	Beams_VertexShader vertexShader3; Beams_FragmentShader fragShader3;
	Advance_TessCtrlShader tessCtrlShader; Advance_TessEvalShader tessEvalShader;
	Advance_GeometryShader geomShader;
};