// Modern Demo Implemented over Topl_App class

#include "Topl_App.hpp"

#include "trees/Pane.hpp"

struct Exec_App : public Topl_App {
    Exec_App(const char* execPath, APP_Backend b) 
		: Topl_App(execPath, "Exec", b){}

	void init() override;
	void loop(double secs, unsigned long frame) override;

	Topl_Camera camera_main;
	Topl_Scene scene_main;
	Topl_Scene scene_overlay;
	// Topl_Image textImage = Topl_Image(_freetypeLib, )

	Geo_FlatSquare textSquare = Geo_FlatSquare(0.25f);

	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 5);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 2);
};