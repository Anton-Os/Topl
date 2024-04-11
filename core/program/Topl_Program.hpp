#include "Platform.hpp"

#include "Topl_Factory.hpp"

// #define MAX_TIMELINE_ATTRIBS 2056
#define TIMELINE_START 0.0 // 0 millisecs will always be start
#define TIMELINE_AT -1.0
#define TIMELINE_END 60.0 // 1 minute will be default end

class Topl_Timeline {
public:
	Topl_Timeline(){}

	// template<class T> void addSequence(T var, std::pair<millisec_t, T> target); // try to templatize this
	void addSequence_vec3f(Vec3f* vec, std::pair<millisec_t, Vec3f> target);
	// Vec3f* getSequence_vec3f(std::pair<millisec_t, Vec3f> target);
	void addSequence_float(float* var, std::pair<millisec_t, float> target);
	// float* getSequence_float(std::pair<millisec_t, float> target);
	void addSequence_double(double* var, std::pair<millisec_t, double> target);
	// double* getSequence_double(std::pair<millisec_t, double> target);

	Timer_Dynamic dynamic_ticker = Timer_Dynamic(TIMELINE_START); // variably incrementing
	Timer_Persist persist_ticker; // constantly incrementing

	static void seqCallback(millisec_t m);
private:
	static std::map<Vec3f*, std::map<millisec_t, Vec3f>> vec3f_map;
	static std::map<float*, std::map<millisec_t, float>> float_map;
	static std::map<double*, std::map<millisec_t, double>> double_map;

	std::vector<Vec3f> vec3f_data;
	std::vector<float> float_data;
	std::vector<double> double_data;
};

#define NO_PICKER_OBJ nullptr
#define CACHED_FRAME_COUNT 60

class Topl_Program {
public:
    Topl_Program(const char* execPath, const char* name, BACKEND_Target backend);
	~Topl_Program(){ }

	void run();
#ifdef RASTERON_H
	unsigned colorPicker(Topl_Scene* scene); // get solid color to find item
	Vec3f coordPicker(Topl_Scene* scene); // get relative coordinates within item
#endif
	static Vec3f getCamCursorPos(){ return (*cameraObj.getPos() + cursorPos - Vec3f{ 0.0, 0.0, CAM_DEPTH } ) * (1.0 / *cameraObj.getZoom()); } // TODO: Include roll

	static Topl_Camera cameraObj; // custom camera object
	static Topl_Timeline timeline;
	static Vec3f cursorPos;
	static bool isCamera_keys, isCamera_mouse; // static bool isCamera_MounseControl;
	static bool isInputEnabled;
	static std::string userInput; // input is added when characters are pressed
#ifdef RASTERON_H
	static unsigned pickerColor; // picker for color
	// static unsigned pickerVal_coord; // picker for coordinates
	static Vec3f pickerCoord;
	static Geo_Actor* pickerObj; // picker for actor
	static Rasteron_Queue* cachedFrames; // frame capture queue
#endif
protected:
    virtual void init() = 0;
    virtual void loop(millisec_t frameTime) = 0;

	void cleanup();

	// Rendering
	const enum BACKEND_Target _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;
};
