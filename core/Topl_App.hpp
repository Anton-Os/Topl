#include "Platform.hpp"

#include "Topl_Factory.hpp"

#define EVENT_HANDLE_TIME 1.0 / 120

class Topl_App {
public:
    Topl_App(const char* execPath, const char* name, APP_Backend backend);
	~Topl_App();

	void run();

	// Paths
	const static std::string getFontsPath() { return std::string(ASSETS_DIR) + "fonts/"; }
	const static std::string getImagesPath() { return std::string(ASSETS_DIR) + "images/"; }
	const static std::string getModelsPath() { return std::string(ASSETS_DIR) + "models/"; }
	const static std::string getResourcesPath() { return std::string(ASSETS_DIR) + "res/"; }
protected:
    virtual void init() = 0;
    virtual void loop(unsigned long frame) = 0;

	// Rendering
	const enum APP_Backend _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;

	// Utility
	Timer_Ticker _ticker;
};