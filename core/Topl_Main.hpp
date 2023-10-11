#include "Platform.hpp"

#include "Topl_Factory.hpp"

class Topl_Main {
public:
    Topl_Main(const char* execPath, const char* name, TARGET_Backend backend);
	~Topl_Main();

	void run();

	// Paths
	const static std::string getFontsPath() { return std::string(ASSETS_DIR) + "fonts/"; }
	const static std::string getImagesPath() { return std::string(ASSETS_DIR) + "images/"; }
	const static std::string getModelsPath() { return std::string(ASSETS_DIR) + "models/"; }
	const static std::string getResourcesPath() { return std::string(ASSETS_DIR) + "res/"; }
protected:
    virtual void init() = 0;
    virtual void loop(double frameTime) = 0;

	// Rendering
	const enum TARGET_Backend _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;

	// Utility
	Timer_Ticker _ticker;
};