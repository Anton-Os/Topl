#include "Platform.hpp"

#include "Topl_Factory.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Effect_Shader.hpp"
#include "Canvas_Shader.hpp"
#include "Experimental_Shader.hpp"
#include "Advance_Shader.hpp"

#include "constructs/Geo_Billboards.hpp"
#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#define NO_PICKER_OBJ nullptr
#define CACHED_FRAME_COUNT 60
#define PROGRAM_M 0.1f
#define PROGRAM_OVERLAYS 3
#define PROGRAM_SCENE Topl_Scene(&Topl_Program::camera)

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

	// These should not be static
	static Topl_Camera camera; // custom camera object
	static Topl_Timeline timeline;
	static Vec3f cursorPos;
	static float speed;
	static unsigned shaderMode;
	static Topl_EntryShader* activeShader; // active shader controlled by the pipeline
	static std::string userInput; // input is added when characters are pressed
	static bool isCtrl_keys, isCtrl_shader, isCtrl_input; // static bool isCamera_MounseControl;

	static Geo_Actor *pickerObj, *lastPickerObj; // picker for actor
	static unsigned pickerColor, lastPickerColor; // picker for color
	static Vec3f pickerCoord, lastPickerCoord;
	static Vec3f getCoordDiff(){ return lastPickerCoord - pickerCoord; }
#ifdef RASTERON_H
	static Rasteron_Queue* cachedFrames; // frame capture queue
#endif
	std::map<Geo_Actor*, Vec3f> positions_map, rotations_map, scales_map; // for dynamic orientation changes
#ifdef RASTERON_H
	std::map<Geo_Actor*, Img_Sequence> sequence_map; // for dynamically changing textures
#endif
    bool isEnable_screencap = true;
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
	Experimental_VertexShader _dynamicVShader; Experimental_PixelShader _dynamicPShader;
	Advance_GeometryShader _geomShader; Advance_TessCtrlShader _tessCtrlShader; Advance_TessEvalShader _tessEvalShader;

	Topl_EntryShader* _entryShaders[6] = { &_texVShader, &_beamsVShader, &_flatVShader, &_effectVShader, &_canvasVShader, &_dynamicVShader };
	Topl_Pipeline *_texPipeline, *_beamsPipeline, *_flatPipeline, *_effectPipeline, *_canvasPipeline, *_dynamicPipeline; // for easy reuse

    // Options & Properties

    void renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, int mode);
	bool isEnable_background = true, isEnable_overlays = true;
private:
    void _onAnyKey(char k);
    void _onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor);

    void setPipelines();

	// Scenes, Geometry & Targets
	struct Background {
        Geo_Quad2D mesh = Geo_Quad2D(2.0F);
        Geo_Actor actor = Geo_Actor("background", &mesh);
        Topl_Camera camera = Topl_Camera();
        Topl_Scene scene = Topl_Scene(&camera, { &actor });
#ifdef RASTERON_H
        Img_Base image = Img_Base(std::string(IMAGES_DIR) + "Background-Action.bmp");
#endif
	} _background;

	void createBackground(Img_Base* backgroundTex);
	void _backgroundCallback(MOUSE_Event event, Geo_Actor* actor);

	struct Editor {
		Geo_Quad3D mesh = Geo_Quad3D(1.0F);
		Geo_Actor actor = Geo_Actor("pickerEdit", &mesh);
#ifdef RASTERON_H
		std::string fontPath = std::string(FONTS_DIR) + "MajorMonoDisplay-Regular.ttf";
		Img_Base nameImg = Img_Base({ fontPath.c_str(), actor.getName().c_str(), 0xFF111111, 0xFFEEEEEE });
		Geo_Quad2D nameMesh = Geo_Quad2D(0.05F);
		Geo_Actor nameActor = Geo_Actor("pickerName", &nameMesh);
		Topl_Scene scene = Topl_Scene(&Topl_Program::camera, { &actor, &nameActor });
#else
		Topl_Scene scene = Topl_Scene(&Topl_Program::camera, { &actor });
#endif
	} _editor;

	struct Overlays {
        Topl_Camera camera;
        Topl_Scene scene = Topl_Scene(&camera);
		Geo_Billboard billboard_camera = Geo_Billboard("prog_camera", 3, 3, &scene);
		Geo_Billboard billboard_object = Geo_Billboard("prog_object", 3, 4, &scene);
		Geo_Billboard billboard_shader = Geo_Billboard("prog_shader", 3, 3, &scene);
		Geo_Billboard* billboards[3] = { &billboard_camera, &billboard_object, &billboard_shader };
#ifdef RASTERON_H
		Img_Button button = Img_Button(MENU_Medium);
        Img_Dial dial = Img_Dial(MENU_Medium, 4);
        Img_Slider slider = Img_Slider(MENU_Medium, 2);
		std::map<Geo_Actor*, Img_Button*> button_map;
#endif
	} _overlays;

	void createOverlays(double size);
	void _overlayCallback(MOUSE_Event event, Geo_Actor* actor);
};
