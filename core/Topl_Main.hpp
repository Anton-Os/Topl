#include "Platform.hpp"

#include "Topl_Factory.hpp"

class Topl_Main {
public:
    Topl_Main(const char* execPath, const char* name, TARGET_Backend backend);
	~Topl_Main();

	void run();
protected:
    virtual void init() = 0;
    virtual void loop(double frameTime) = 0;

#ifdef RASTERON_H
	unsigned colorPicker(Topl_Scene* scene);
#endif

	// Rendering
	const enum TARGET_Backend _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;

	// Timing // TODO: Include internal timeline management
	Timer_Ticker _ticker;
};