#include "Platform.hpp"

#include "Topl_Factory.hpp"

#define MAX_TIMELINE_ATTRIBS 2056
#define TIMELINE_START 0.0 // 0 millisecs will always be start
#define TIMELINE_END 60000.0 // 1 minute will be default end

class Topl_Timeline {
public:
	Topl_Timeline();

	void addSequence_float(float* var, std::pair<millisec_t, float> target);
	// void addSequence_double(double* var, std::pair(millisec_t, double));
	// void addSequence_int(int* var, std::pair(millisec_t, double));

	Timer_Dynamic dynamic_ticker = Timer_Dynamic(TIMELINE_START); // variably incrementing
	Timer_Persist persist_ticker; // constantly incrementing

	static void seqCallback(millisec_t m);
private:
	static std::map<float*, std::map<millisec_t, float>> float_map;
	// std::map<double*, std::map<millisec_t, double>> _double_map;
	// std::map<int*, std::map<millisec_t, int>> _int_map;

	millisec_t _elapseTime = TIMELINE_START;
	millisec_t _elapseRange[2] = { TIMELINE_START, TIMELINE_END };
};

#define NO_PICKER_OBJ nullptr
#define CACHED_FRAME_COUNT 60

class Topl_Program {
public:
    Topl_Program(const char* execPath, const char* name, BACKEND_Target backend);
	~Topl_Program();

	void run();
#ifdef RASTERON_H
	unsigned colorPicker(Topl_Scene* scene); // get solid color to find item
	Vec3f coordPicker(Topl_Scene* scene); // get relative coordinates within item
#endif
	static Vec3f getCamCursorPos(){ return (*cameraObj.getPos() + cursorPos - Vec3f{ 0.0, 0.0, -1.0} ) * (1.0 / *cameraObj.getZoom()); } // TODO: Include roll

	static Vec3f cursorPos;
	static bool isCamera_KeyControl; // static bool isCamera_MounseControl;
	static bool isInputEnabled;
	static std::string userInput; // input is added when characters are pressed
#ifdef RASTERON_H
	static unsigned pickerVal_color; // picker for color
	static unsigned pickerVal_coord; // picker for coordinates
	static const Geo_Actor* pickerObj; // picker for actor
	static Rasteron_Queue* cachedFrames; // frame capture queue
#endif
	static Topl_Camera cameraObj; // custom camera object

	static Topl_Timeline timeline;
protected:
    virtual void init() = 0;
    virtual void loop(millisec_t frameTime) = 0;

	// Rendering
	const enum BACKEND_Target _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
};