#include "Topl_App.hpp"

#include "Textured_Shader.hpp"
#include "Flat_Shader.hpp"
#include "Beams_Shader.hpp"

#include "primitives/Geo_Conic.hpp"
#include "primitives/Geo_Extruded.hpp"
#include "primitives/Geo_Sphere.hpp"
#include "trees/Geo_Chain.hpp"
#include "trees/Geo_Grid.hpp"
#include "trees/Geo_Pane.hpp"
#include "trees/Geo_Humanoid.hpp"

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

std::string devilAssets[HUMANOID_PARTS_COUNT] = {
	Topl_App::getImagesPath() + "Demon-Head.png",
	Topl_App::getImagesPath() + "Demon-LeftWing.png",
	Topl_App::getImagesPath() + "Demon-RightWing.png",
	Topl_App::getImagesPath() + "Demon-Torso.png",
	Topl_App::getImagesPath() + "Demon-LeftLeg.png",
	Topl_App::getImagesPath() + "Demon-RightLeg.png"
};

Img_Base yellowImg = Img_Base(0xFFFFFF00);
Img_Base characterImages[HUMANOID_PARTS_COUNT]; // = { Img_Base(radialImg), Img_Base(radialImg), Img_Base(radialImg), Img_Base(radialImg), Img_Base(radialImg), Img_Base(radialImg) };
Img_Base ghostImages[HUMANOID_PARTS_COUNT], angelImages[HUMANOID_PARTS_COUNT], devilImages[HUMANOID_PARTS_COUNT];
Geo_Humanoid2D character = Geo_Humanoid2D("character", noAssets);

struct Playground_App : public Topl_App {
    Playground_App(const char* execPath, APP_Backend backend) 
		: Topl_App(execPath, "Playground", backend){}

	void init() override;
	void loop(unsigned long frame) override;

	// Configurations

	static Topl_Camera camera1, camera2, camera3;
	static Topl_Light skyLight, flashLight, lampLight;
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
	Geo_Actor platformActor = Geo_Actor((Geo_Renderable*)&square);
	Geo_Sphere sphere = Geo_Sphere({ 0.025f, 200, 200 });
	Geo_Actor sphereActors[PLAYGROUND_SPHERES_COUNT];

	// Overlay Scene Elements
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 9, PANE_RADIUS, 0.05f);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 3, PANE_RADIUS, 0.05f);

	// Detail Scene Elements
	Geo_ExtrudedTriangle prism = Geo_ExtrudedTriangle(0.2f); 
	Geo_ConicCircle cone = Geo_ConicCircle(0.2f, Vec3f({ 0.1f, 0.1f, 0.3f }));
	Geo_Actor prismActor = Geo_Actor((Geo_Renderable*)&prism);
	Geo_Actor coneActor = Geo_Actor((Geo_Renderable*)&cone);
	// Geo_Grid_Params gridParams = Geo_Grid_Params(std::make_pair(10, 0.25f), std::make_pair(1, 0.0f), std::make_pair(10, 0.1f));
	// Geo_Grid grid = Geo_Grid("grid", &coneActor1, gridParams);

	// Textures, Materials, Heightmaps

	Img_Base boxedPaneImages[PLAYGROUND_PANE_COUNT];
	std::string fontStr = std::string(Topl_App::getFontsPath() + "MajorMonoDisplay-Regular.ttf");
	Rasteron_Text boxedTextObjs[PLAYGROUND_PANE_COUNT];
	// Img_Volume volume = Img_Volume("volume", 4096, 4096, 256);
	/* Img_Base heightmapImage = Img_Base(Topl_App::getImagesPath() + "BigGrid.png");
	Img_Heightmap heightmap = Img_Heightmap(heightmapImage.getImage());
	Geo_Actor heightmapActor = Geo_Actor((Geo_Renderable*)&heightmap); */

	// Shaders and Pipelines

	Topl_Pipeline *colPipeline, *texPipeline, *litPipeline;

	Textured_VertexShader vertexShader1; Textured_FragmentShader fragShader1;
	Flat_VertexShader vertexShader2; Flat_FragmentShader fragShader2;
	Beams_VertexShader vertexShader3; Beams_FragmentShader fragShader3;
};