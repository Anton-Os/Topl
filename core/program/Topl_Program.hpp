#include "Platform.hpp"

#include "Topl_Factory.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Effect_Shader.hpp"
#include "Canvas_Shader.hpp"
#include "Dynamic_Shader.hpp"
#include "Advance_Shader.hpp"

#include "constructs/Geo_Billboards.hpp"
#include "meshes/Geo_Surface.hpp"

#define NO_PICKER_OBJ nullptr
#define CACHED_FRAME_COUNT 60
#define PROGRAM_M 0.1f
#define PROGRAM_OVERLAYS 3

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
	void addSequence_float(float* var, std::pair<millisec_t, float> target);
	void addSequence_double(double* var, std::pair<millisec_t, double> target);

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

class Topl_Program {
public:
#ifndef __ANDROID__
    Topl_Program(const char* execPath, const char* name, BACKEND_Target backend);
#else
	Topl_Program(android_app* app);
#endif
	~Topl_Program(){ cleanup(); }

	void run();
#ifdef RASTERON_H
	bool checkPicker(Geo_Actor* actor){ return pickerObj->getId() == actor->getId(); }
	unsigned colorPicker(Topl_Scene* scene); // get solid color to find item
	Vec3f coordPicker(Topl_Scene* scene); // get relative coordinates within item
#endif
	static Vec3f getCamRelPos(Topl_Camera* cam){ 
		if(cam == nullptr) cam = &camera;
		return (*cam->getPos() + Vec3f({ Platform::getCursorX(), Platform::getCursorY(), 0.0 }) - Vec3f{ 0.0, 0.0, CAM_DEPTH } ) * (1.0 / *cam->getZoom());  // TODO: Include roll
	}

	static Topl_Camera camera; // custom camera object
	static Topl_Timeline timeline;
	static Vec3f cursorPos;
	static float speed;
	static unsigned shaderMode;
	static Topl_EntryShader* activeShader; // active shader controlled by the pipeline
	static std::string userInput; // input is added when characters are pressed
	static bool isCtrl_keys, isCtrl_shader, isCtrl_input; // static bool isCamera_MounseControl;

	static Geo_Actor* pickerObj; // picker for actor
	static unsigned pickerColor, lastPickerColor; // picker for color
	static Vec3f pickerCoord, lastPickerCoord;
	static Vec3f getCoordDiff(){ return lastPickerCoord - pickerCoord; }
#ifdef RASTERON_H
	static Rasteron_Queue* cachedFrames; // frame capture queue
#endif
	std::map<Geo_Actor*, Vec3f> positions_map, rotations_map, scales_map; // for dynamic orientation changes
#ifdef RASTERON_H
	std::map<Geo_Actor*, Img_Sequence> sequence_map; // for dynamically changing textures
	bool isEnable_screencap = true;
#endif
protected:
    virtual void init() = 0;
	virtual void preloop();
    virtual void loop(millisec_t frameTime) = 0;
	virtual void postloop();

	void cleanup();

	// Rendering
	const enum BACKEND_Target _backend;
	Topl_Renderer* _renderer = nullptr;
	Platform* _platform;

	// Shaders and Pipelines

	Textured_VertexShader _texVShader; Textured_PixelShader _texPShader;
	Beams_VertexShader _beamsVShader; Beams_PixelShader _beamsPShader;
	Flat_VertexShader _flatVShader; Flat_PixelShader _flatPShader;
	Effect_VertexShader _effectVShader; Effect_PixelShader _effectPShader;
	Canvas_VertexShader _canvasVShader; Canvas_PixelShader _canvasPShader;
	Dynamic_VertexShader _dynamicVShader; Dynamic_PixelShader _dynamicPShader;
	Advance_GeometryShader _geomShader; Advance_TessCtrlShader _tessCtrlShader; Advance_TessEvalShader _tessEvalShader;

	Topl_EntryShader* _entryShaders[6] = { &_texVShader, &_beamsVShader, &_flatVShader, &_effectVShader, &_canvasVShader, &_dynamicVShader };
	Topl_Pipeline *_texPipeline, *_beamsPipeline, *_flatPipeline, *_effectPipeline, *_canvasPipeline, *_dynamicPipeline; // for easy reuse

    // Options & Properties

    bool isEnable_background = true, isEnable_overlays = true;
private:
    void renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, unsigned short mode);

	// Scenes, Geometry & Targets
	struct Background {
        Geo_Quad2D mesh = Geo_Quad2D(2.0F);
        Geo_Actor actor = Geo_Actor("background", &mesh);
        Topl_Camera camera = Topl_Camera();
        Topl_Scene scene = Topl_Scene({ &actor });
	} _background;

	struct Overlays {
        Topl_Camera camera;
        Topl_Scene scene;
		Geo_Billboard billboard_camera = Geo_Billboard("prog_camera", 2, 3, &scene);
		Geo_Billboard billboard_object = Geo_Billboard("prog_object", 2, 3, &scene);
		Geo_Billboard billboard_shader = Geo_Billboard("prog_shader", 2, 3, &scene);
		Geo_Billboard* billboards[3] = { &billboard_camera, &billboard_object, &billboard_shader };
#ifdef RASTERON_H
		Img_Button button = Img_Button(MENU_Medium);
#endif
	} _overlays;
};
