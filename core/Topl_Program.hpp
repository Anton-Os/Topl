#include "Platform.hpp"

#include "Topl_Factory.hpp"

#define MAX_TIMELINE_ATTRIBS 256

class Topl_Timeline {
public:
	Topl_Timeline(){}

	void addSequence_float(float* var, std::pair<millisec_t, float> timeTarget);
	// void addSequence_double(double* var, std::pair(millisec_t, double));
	// void addSequence_int(int* var, std::pair(millisec_t, double));

	Timer_Ticker ticker;
private:
	std::map<float*, std::map<millisec_t, float>> _float_map;
	// std::map<double*, std::map<millisec_t, double>> _double_map;
	// std::map<int*, std::map<millisec_t, int>> _int_map;
};

#define NO_PICKER_OBJ nullptr

class Topl_Program {
public:
    Topl_Program(const char* execPath, const char* name, BACKEND_Target backend);
	~Topl_Program();

	void run();
#ifdef RASTERON_H
	unsigned invokePicker(Topl_Scene* scene);
#endif

	static Vec3f cursorPos;
	static std::string userInput;
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