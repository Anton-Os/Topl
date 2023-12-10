#include "Platform.hpp"

#include "Topl_Factory.hpp"

#define MAX_TIMELINE_ATTRIBS 256

class Topl_Timeline {
public:
	Topl_Timeline(){
		// TODO: Add event timer for calculating transforms
	}

	// void addTform_float(float* target, std::pair(millisec_t, float));
	// void addTform_double(double* target, std::pair(millisec_t, double));
	// void addTform_int(int* target, std::pair(millisec_t, double));

	Timer_Ticker ticker;
private:
	std::map<float*, std::map<millisec_t, float>> tformTable_float;
	std::map<double*, std::map<millisec_t, double>> tformTable_double;
	std::map<int*, std::map<millisec_t, int>> tformTable_int;
};

#define NO_PICKER_OBJ nullptr

class Topl_Main {
public:
    Topl_Main(const char* execPath, const char* name, BACKEND_Target backend);
	~Topl_Main();

	void run();
#ifdef RASTERON_H
	unsigned invokePicker(Topl_Scene* scene);
#endif

	static Vec3f cursorPos;
#ifdef RASTERON_H
	static unsigned pickerVal; // picker for color
	static const Geo_Actor* pickerObj; // picker for actor
#endif
protected:
    virtual void init() = 0;
    virtual void loop(millisec_t frameTime) = 0;

	Topl_Timeline _timeline;

	// Rendering
	const enum BACKEND_Target _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
};