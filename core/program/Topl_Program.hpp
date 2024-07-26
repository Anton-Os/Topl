#include "Platform.hpp"

#include "Topl_Factory.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Effect_Shader.hpp"

// #define MAX_TIMELINE_ATTRIBS 2056
#define TIMELINE_START 0.0 // 0 millisecs will always be start
#define TIMELINE_AT -1.0
#define TIMELINE_END 60.0 // 1 minute will be default end

class Topl_Timeline {
public:
	Topl_Timeline(){}

	template<typename T> void addSequence(T var, std::pair<millisec_t, T> target);
	// template<class T> void addSequence(T var, std::pair<millisec_t, T> target); // try to templatize this
	void addSequence_vec3f(Vec3f* vec, std::pair<millisec_t, Vec3f> target);
	// Vec3f* getSequence_vec3f(std::pair<millisec_t, Vec3f> target);
	void addSequence_float(float* var, std::pair<millisec_t, float> target);
	// float* getSequence_float(std::pair<millisec_t, float> target);
	void addSequence_double(double* var, std::pair<millisec_t, double> target);
	// double* getSequence_double(std::pair<millisec_t, double> target);

	Timer_Dynamic dynamic_ticker = Timer_Dynamic(TIMELINE_START); // variably incrementing
	Timer_Persist persist_ticker; // constantly incrementing

	template<typename T> void updateSequence(T, millisec_t);
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
	bool checkPicker(Geo_Actor* actor){ return pickerObj->getId() == actor->getId(); }
	unsigned colorPicker(Topl_Scene* scene); // get solid color to find item
	Vec3f coordPicker(Topl_Scene* scene); // get relative coordinates within item
#endif
	static Vec3f getCamRelPos(Topl_Camera* camera){ 
		if(camera == nullptr) camera = &cameraObj;
		return (*camera->getPos() + Vec3f({ Platform::getCursorX(), Platform::getCursorY(), 0.0 }) - Vec3f{ 0.0, 0.0, CAM_DEPTH } ) * (1.0 / *camera->getZoom());  // TODO: Include roll
	}

	static Topl_Camera cameraObj; // custom camera object
	static Topl_Timeline timeline;
	static Vec3f cursorPos;
	static bool isCamera_keys, isCamera_mouse; // static bool isCamera_MounseControl;
	static bool isInputEnabled;
	static std::string userInput; // input is added when characters are pressed
#ifdef RASTERON_H
	static Geo_Actor* pickerObj; // picker for actor
	static unsigned pickerColor, lastPickerColor; // picker for color
	static Vec3f pickerCoord, lastPickerCoord;
	static Vec3f getCoordDiff(){ return lastPickerCoord - pickerCoord; }
	static Rasteron_Queue* cachedFrames; // frame capture queue
#endif
	std::map<Geo_Actor*, Vec3f> positions_map, rotations_map, scales_map; // for dynamic orientation changes
protected:
    virtual void init() = 0;
    virtual void loop(millisec_t frameTime) = 0;

	void cleanup();

	// Rendering
	const enum BACKEND_Target _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;

	// std::map<std::string, Topl_Scene> _scenes_map; // for custom scene
#ifdef RASTERON_H
	std::map<Img_Base*, std::pair<Rasteron_Queue*, unsigned short>> _images_map; // for dynamic texture changes
#endif

	Topl_Pipeline *_texPipeline, *_beamsPipeline, *_flatPipeline, *_effectPipeline; // for easy reuse

	Textured_VertexShader _texVShader; Textured_PixelShader _texPShader;
	Beams_VertexShader _beamsVShader; Beams_PixelShader _beamsPShader;
	Flat_VertexShader _flatVShader; Flat_PixelShader _flatPShader;
	Effect_VertexShader _effectVShader; Effect_PixelShader _effectPShader;
};
