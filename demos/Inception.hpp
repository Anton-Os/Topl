#include "Topl_App.hpp"

#include "trees/Pane.hpp"

struct Inception_App : public Topl_App {
    Inception_App(const char* execPath, APP_Backend b) 
		: Topl_App(execPath, "Inception", b){}

	void init() override;
	void loop(double secs, unsigned long frame) override;

	Topl_Camera camera_main;
	Topl_Scene scene_main;
	Topl_Scene scene_overlay;

	Geo_RowLayout rowLayout = Geo_RowLayout("Rows", 5);
	Geo_BoxedLayout boxedLayout = Geo_BoxedLayout("Boxes", 2);
};