#include "Platform.hpp"

#include "Topl_Factory.hpp"

#define EVENT_HANDLE_TIME 1.0 / 120

// LOOK FOR WHERE BOTTLENECK OCCURS!
class Topl_App {
public:
    Topl_App(const char* execPath, const char* name, APP_Backend backend);
	~Topl_App();

	void run();
protected:
    virtual void init() = 0;
    virtual void loop(unsigned long frame) = 0;

	// Rendering
	const enum APP_Backend _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
	const Topl_Pipeline* _activePipeline = nullptr;

	// Utility
    Timer_Ticker _ticker;

	// Paths
	const std::string fontsPath = std::string(ASSETS_DIR) + "fonts/";
	const std::string imagesPath = std::string(ASSETS_DIR) + "images/";
	const std::string modelsPath = std::string(ASSETS_DIR) + "models/";
	const std::string resourcePath = std::string(ASSETS_DIR) + "res/";
};