// Modern Demo Implemented over Topl_App class

#include "Topl_App.hpp"

#include "Flat_Shader.hpp"
#include "Advance_Shader.hpp"

#include "trees/Pane.hpp"

struct Exec_App : public Topl_App {
    Exec_App(const char* execPath, APP_Backend b) 
		: Topl_App(execPath, "Exec", b){}

	void init() override;
	void loop(double secs, unsigned long frame) override;

	// Configurations

	Topl_Camera camera_main;
	Topl_Scene scene_main;
	Topl_Scene scene_overlay;

	// Geometries

	Geo_FlatTriangle triangle = Geo_FlatTriangle(0.25f);
	Geo_Actor triangleActor = Geo_Actor((Geo_RenderObj*)&triangle);
	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 5);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 2);

	// Shaders and Pipelines

	Topl_Pipeline* flatPipeline;
	Topl_Pipeline* advancePipeline;

	Flat_VertexShader vertexShader;
	Flat_FragmentShader fragShader;
	Advance_TessCtrlShader tessCtrlShader;
	Advance_TessEvalShader tessEvalShader;
	Advance_GeometryShader geomShader;
};