#include "Platform.hpp"

#include "Topl_Factory.hpp"

#define MAX_TIMELINE_ATTRIBS 2056
#define TIMELINE_START 0.0 // 0 millisecs will always be start
#define TIMELINE_END 60000.0 // 1 minute will be default end

class Topl_Timeline {
public:
	Topl_Timeline();

	void addSequence_float(float* var, std::pair<millisec_t, float> timeTarget);
	// void addSequence_double(double* var, std::pair(millisec_t, double));
	// void addSequence_int(int* var, std::pair(millisec_t, double));

	Timer_Ticker ticker; // controls global events
	Timer_Dynamic dynamic_ticker = Timer_Dynamic(TIMELINE_START); // controls context sensitive events
private:
	std::map<float*, std::map<millisec_t, float>> _float_map;
	// std::map<double*, std::map<millisec_t, double>> _double_map;
	// std::map<int*, std::map<millisec_t, int>> _int_map;

	millisec_t _elapseTime = TIMELINE_START;
	millisec_t _elapseRange[2] = { TIMELINE_START, TIMELINE_END };
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
	static Vec3f getCamCursorPos(){ return (*cameraObj.getPos() + cursorPos) * (1.0 / *cameraObj.getZoom()); } // TODO: Include roll

	static Vec3f cursorPos;
	static std::string userInput; // input is added when characters are pressed
#ifdef RASTERON_H
	static unsigned pickerVal; // picker for color
	static const Geo_Actor* pickerObj; // picker for actor
#endif
	static Topl_Camera cameraObj; // custom camera object
protected:
    virtual void init() = 0;
    virtual void loop(millisec_t frameTime) = 0;

	Topl_Timeline _timeline;

	// Rendering
	const enum BACKEND_Target _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
};