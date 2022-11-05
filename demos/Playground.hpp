#include "Topl_App.hpp"

#include "Textured_Shader.hpp"
#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"

#include "Primitives/Geo_Conic.hpp"
#include "primitives/Geo_Sphere.hpp"
#include "trees/Geo_Sequence.hpp"
#include "trees/Geo_Grid.hpp"
#include "trees/Pane.hpp"
#include "trees/Humanoid.hpp"

#define PLAYGROUND_SPHERES_COUNT 6
#define PLAYGROUND_PANE_COUNT 9

std::string noAssets[HUMANOID_PARTS_COUNT]{ "", "", "", "", "", "" };

std::string ghostAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::getImagesPath() + "Ghost-Head.png",
	Topl_App::getImagesPath() + "Ghost-LeftArm.png",
	Topl_App::getImagesPath() + "Ghost-RightArm.png",
	Topl_App::getImagesPath() + "Ghost-Torso.png",
	Topl_App::getImagesPath() + "Ghost-LeftLeg.png",
	Topl_App::getImagesPath() + "Ghost-RightLeg.png"
};

std::string angelAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::getImagesPath() + "Angel-Head.png",
	Topl_App::getImagesPath() + "Angel-LeftWing.png",
	Topl_App::getImagesPath() + "Angel-RightWing.png",
	Topl_App::getImagesPath() + "Angel-Torso.png",
	Topl_App::getImagesPath() + "Angel-LeftLeg.png",
	Topl_App::getImagesPath() + "Angel-RightLeg.png"
};

std::string demonAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::getImagesPath() + "Demon-Head.png",
	Topl_App::getImagesPath() + "Demon-LeftWing.png",
	Topl_App::getImagesPath() + "Demon-RightWing.png",
	Topl_App::getImagesPath() + "Demon-Torso.png",
	Topl_App::getImagesPath() + "Demon-LeftLeg.png",
	Topl_App::getImagesPath() + "Demon-RightLeg.png"
};

Geo_Humanoid2D character = Geo_Humanoid2D("character", ghostAssets);

struct Playground_App : public Topl_App {
    Playground_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Playground", backend){}

	void init() override;
	void loop(unsigned long frame) override;

	// Configurations

	static Topl_Camera camera1, camera2, camera3;
	Topl_Scene scene_main, scene_overlay, scene_details;

private:
	void preFrame();
	void postFrame();

	void createPipeline();
	void createScene_Main();
	void createScene_Overlay();
	void createScene_Details();

	// Main Scene Elements
	Geo_FlatSquare square = Geo_FlatSquare(1.0f);
	Geo_Actor platformActor = Geo_Actor((Geo_RenderObj*)&square);
	Geo_Sphere sphere = Geo_Sphere({ 0.025f, 200, 200 });
	Geo_Actor sphereActors[PLAYGROUND_SPHERES_COUNT];

	// Overlay Scene Elements
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 9);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 3);

	// Detail Scene Elements
	Geo_ConicTriangle cone1 = Geo_ConicTriangle(0.2f); 
	Geo_ConicHex cone2 = Geo_ConicHex(0.2f, Vec3f({ 0.1f, 0.1f, 0.3f }));
	Geo_Actor coneActor1 = Geo_Actor((Geo_RenderObj*)&cone1);
	Geo_Actor coneActor2 = Geo_Actor((Geo_RenderObj*)&cone2);
	// Geo_Grid_Params gridParams = Geo_Grid_Params(std::make_pair(10, 0.25f), std::make_pair(1, 0.0f), std::make_pair(10, 0.1f));
	// Geo_Grid grid = Geo_Grid("grid", &coneActor1, gridParams);

	// Textures, Materials, Heightmaps

	Rasteron_Image* frameImage = nullptr;
	Img_Base boxedPaneImages[PLAYGROUND_PANE_COUNT];
	std::string fontStr = std::string(Topl_App::getFontsPath() + "MajorMonoDisplay-Regular.ttf");
	Rasteron_Text boxedTextObjs[PLAYGROUND_PANE_COUNT];
	Img_Material material = Img_Material("material1", 4096, 4096);
	/* Img_Base heightmapImage = Img_Base(Topl_App::getImagesPath() + "BigGrid.png");
	Img_Heightmap heightmap = Img_Heightmap(heightmapImage.getImage());
	Geo_Actor heightmapActor = Geo_Actor((Geo_RenderObj*)&heightmap); */

	// Shaders and Pipelines

	Topl_Pipeline *colPipeline, *texPipeline, *litPipeline;

	Textured_VertexShader vertexShader1; Textured_FragmentShader fragShader1;
	Flat_VertexShader vertexShader2; Flat_FragmentShader fragShader2;
	Beams_VertexShader vertexShader3; Beams_FragmentShader fragShader3;
};