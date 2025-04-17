#include "Platform.hpp"

#include "Topl_Factory.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"
#include "Material_Shader.hpp"
#include "Effect_Shader.hpp"
#include "Canvas_Shader.hpp"
#include "Pattern_Shader.hpp"
#include "Advance_Shader.hpp"

#include "constructs/Geo_Billboards.hpp"
#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#define NO_PICKER_OBJ nullptr
#define CACHED_FRAME_COUNT 60
#define PROGRAM_M 0.1f
#define PROGRAM_SCENE Topl_Scene(&Topl_Program::camera)
#define PROGRAM_PIPELINES 7
#define PROGRAM_BK_TESS 3
#define PROGRAM_BK_SIZE 5.0F
#define PROGRAM_BILLBOARDS 8
#define PROGRAM_SUBMENUS 9

// #define MAX_TIMELINE_ATTRIBS 2056
#define TIMELINE_START 0.0 // 0 millisecs will always be start
#define TIMELINE_AT -1.0
#define TIMELINE_END 60.0 // 1 minute will be default end
#define TIMELINE_FORETELL -0.5

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
	static unsigned short mode;
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
	std::map<Geo_Actor*, Sampler_Array> sequence_map; // for dynamically changing textures
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
	Material_VertexShader _materialVShader; Material_PixelShader _materialPShader;
	Flat_VertexShader _flatVShader; Flat_PixelShader _flatPShader;
	Effect_VertexShader _effectVShader; Effect_PixelShader _effectPShader;
	Canvas_VertexShader _canvasVShader; Canvas_PixelShader _canvasPShader;
	Pattern_VertexShader _patternVShader; Pattern_PixelShader _patternPShader;
	Advance_GeometryShader _geomShaders[PROGRAM_PIPELINES]; Advance_TessCtrlShader _tessCtrlShaders[PROGRAM_PIPELINES]; Advance_TessEvalShader _tessEvalShaders[PROGRAM_PIPELINES];

	Topl_EntryShader* _entryShaders[PROGRAM_PIPELINES] = { &_texVShader, &_beamsVShader, &_materialVShader, &_flatVShader, &_effectVShader, &_canvasVShader, &_patternVShader };
	Topl_Pipeline *_texPipeline, *_beamsPipeline, *_materialPipeline, *_flatPipeline, *_effectPipeline, *_canvasPipeline, *_patternPipeline; // for easy reuse
	Topl_Pipeline *_geomPipeline, *_tessPipeline, *_longPipeline;

    // Options & Properties
	enum PROGRAM_Menu { PROGRAM_Media = 0, PROGRAM_Object = 1, PROGRAM_Shader = 2, PROGRAM_Timeline = 3, PROGRAM_Camera = 4, PROGRAM_AppBar = 5, PROGRAM_Sculpt = 6, PROGRAM_Paint = 7, };

    void renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, int mode);
	virtual void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){ std::cout << "Pane " << std::to_string((int)menu) << " pressed, index " << std::to_string(paneIndex) << std::endl; }

	bool isEnable_background = true, isEnable_overlays = true;
private:
	static Topl_Pipeline* _savedPipeline;

    void _onAnyKey(char k);
    void _onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor);

    void setPipelines();
	void setShadersMode(unsigned m){ for(unsigned s = 0; s < PROGRAM_PIPELINES; s++) _entryShaders[s]->setMode(m); }
	void updateTimelines();
	void updatePipelines();

	Vec3f _camPos = *camera.getPos(); 
	Vec3f _camRot = *camera.getRot(); 
	float _camZoom = *camera.getZoom();

	// Scenes, Geometry & Targets
	struct Background {
		Geo_Mesh meshes[9] = { 
			Geo_Quad2D(PROGRAM_BK_SIZE), Geo_Quad2D(PROGRAM_BK_SIZE * 2), Geo_Quad2D(PROGRAM_BK_SIZE * 5),
			// Geo_Hex2D(PROGRAM_BK_SIZE), Geo_Hex2D(PROGRAM_BK_SIZE * 2), Geo_Hex2D(PROGRAM_BK_SIZE * 5),
			Geo_Trig2D(PROGRAM_BK_SIZE), Geo_Surface({ PROGRAM_BK_SIZE, 5 }, DEFAULT_Z), Geo_Surface({ PROGRAM_BK_SIZE, 7 }, DEFAULT_Z),
			// Geo_Circle2D(PROGRAM_BK_SIZE), Geo_Circle2D(PROGRAM_BK_SIZE * 2), Geo_Circle2D(PROGRAM_BK_SIZE * 5)
			Geo_Hex2D(PROGRAM_BK_SIZE), Geo_Surface({ PROGRAM_BK_SIZE, 8 }, DEFAULT_Z), Geo_Circle2D(PROGRAM_BK_SIZE)
		};
        Geo_Mesh* mesh = &meshes[0];
        Geo_Actor actor = Geo_Actor("program_background", mesh);
        Topl_Camera camera = Topl_Camera();
        Topl_Scene scene = Topl_Scene(&camera, { &actor });
#ifdef RASTERON_H
        Sampler_2D image = Sampler_Gradient(SIDE_Radial, 0xFF111111, 0xFFEEEEEE); // = Sampler_File(std::string(IMAGES_DIR) + "Background-Action.bmp");
		Sampler_3D volumeImg = Sampler_3D(256); // Sampler_3D(SAMPLER_WIDTH, SAMPLER_HEIGHT, 32);
#endif
	} _background;

	void createBackground(Sampler_2D* backgroundTex);
	void _backgroundCallback(MOUSE_Event event, Geo_Actor* actor);

	struct Editor {
		Geo_Quad3D mesh = Geo_Quad3D(1.0F);
		Geo_Actor actor = Geo_Actor("pickerEdit", &mesh);
#ifdef RASTERON_H
		std::string fontPath = std::string(FONTS_DIR) + "CutiveMono-Regular.ttf";
		Sampler_Text nameImg = Sampler_Text({ fontPath.c_str(), /* actor.getName().c_str() */ "1", 0xFF111111, 0xFFEEEEEE });
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
		Geo_Crossboard billboard_camera = Geo_Crossboard("program_camera", 9, &scene);
		Geo_Crossboard billboard_appbar = Geo_Crossboard("program_appbar", 9, &scene);
		Geo_Listboard billboard_sculpt = Geo_Listboard("program_sculpt_bar", 9, &scene);
		Geo_Listboard billboard_paint = Geo_Listboard("program_paint_bar", 9, &scene);
		Geo_Paneboard billboard_timeline = Geo_Paneboard("program_timeline", &scene);
		Geo_Billboard billboard_media = Geo_Billboard("program_media", 3, 2, &scene);
		Geo_Billboard billboard_object = Geo_Billboard("program_object", 5, 3, &scene);
		Geo_Billboard billboard_shader = Geo_Billboard("program_shader", 3, 3, &scene);
		Geo_Billboard* billboards[PROGRAM_BILLBOARDS] = { &billboard_media, &billboard_object, &billboard_shader, &billboard_timeline, &billboard_camera, &billboard_appbar, &billboard_sculpt, &billboard_paint, };
#ifdef RASTERON_H
		std::string fontPath = std::string(FONTS_DIR) + "Raleway-Regular.ttf";
		Sampler_2D textures[8]; // for switching textures
		Sampler_Button button = Sampler_Button();
        Sampler_Dial dials[3] = { Sampler_Dial(4), Sampler_Dial(4), Sampler_Dial(4) };
        Sampler_Slider timeSlider = Sampler_Slider(MENU_XL, (unsigned)TIMELINE_END); // sliders[1] = { Sampler_Slider(2) };
		Sampler_Slider sizeSliders[3] = { Sampler_Slider(3), Sampler_Slider(3), Sampler_Slider(3) };
		// std::pair<Sampler_Button, Sampler_Button> plusMinusButtons = std::make_pair(Sampler_Button("add-square"), Sampler_Button("subtract-square"));
		Sampler_Button numberButtons[PROGRAM_SUBMENUS] = {
			Sampler_Button("paginate-filter-1"), Sampler_Button("paginate-filter-2"), Sampler_Button("paginate-filter-3"),
			Sampler_Button("paginate-filter-4"), Sampler_Button("paginate-filter-5"), Sampler_Button("paginate-filter-6"),
			Sampler_Button("paginate-filter-7"), Sampler_Button("paginate-filter-8"), Sampler_Button("paginate-filter-9")
		};
		Sampler_Button pipelineButtons[PROGRAM_SUBMENUS] = {
			Sampler_Button("app-window"), Sampler_Button("app-window-edit"), Sampler_Button("app-window-module"),
			Sampler_Button("app-window-flash"), Sampler_Button("app-window-star-1"), Sampler_Button("app-window-next"),
			Sampler_Button("app-window-flag"), Sampler_Button("app-window-settings"), Sampler_Button("app-window-clock")
		};
		Sampler_Button sculptButtons[PROGRAM_SUBMENUS] = {
			Sampler_Button("tools-hammer"), Sampler_Button("tools-hammer-1"), Sampler_Button("tools-hammer-3"),
			Sampler_Button("tools-palette-trowel"), Sampler_Button("tools-flattener-triangle"), Sampler_Button("tools-saw"),
			Sampler_Button("tools-pliers"), Sampler_Button("tools-vice-grip"), Sampler_Button("tools-nail-gun")
		};
		Sampler_Button paintButtons[PROGRAM_SUBMENUS] = {
			Sampler_Button("content-pen"), Sampler_Button("content-pen-1"), Sampler_Button("content-pen-3"),
			Sampler_Button("content-pen-6"), Sampler_Button("content-ink-pen"), Sampler_Button("design-tool-quill-1"),
			Sampler_Button("color-rolling-brush"), Sampler_Button("design-tool-paper-glue"), Sampler_Button("color-bucket-brush")
		};
		Sampler_Button cameraButtons[3] = { Sampler_Button("camera-retro"), Sampler_Button("camera-1"), Sampler_Button("camera-2") };
		Sampler_Label mediaLabels[3] = { Sampler_Label({ fontPath.c_str(), "|00|", 0xFF111111, 0xFFEEEEEE }), Sampler_Label({ fontPath.c_str(), "|00|", 0xFF111111, 0xFFEEEEEE }), Sampler_Label({ fontPath.c_str(), "|00|", 0xFF111111, 0xFFEEEEEE }) };
		Sampler_Button mediaButtons[3] = { Sampler_Button("controls-forward"), Sampler_Button("controls-next"), Sampler_Button("controls-rewind") };
		// Sampler_Label timeLabel = Sampler_Label({ fontPath.c_str(), "0:00:00", 0xFF111111, 0xFFEEEEEE });
		Sampler_Label recordLabel = Sampler_Label({ fontPath.c_str(), "RECORD", 0xFF111111, 0xFFEEEEEE });
		Sampler_Label axisLabels[3] = { Sampler_Label({ fontPath.c_str(), "| X |", 0xFF333333, 0xFFEEEEEE }), Sampler_Label({ fontPath.c_str(), "| Y |", 0xFF333333, 0xFFEEEEEE }), Sampler_Label({ fontPath.c_str(), "| Z |", 0xFF333333, 0xFFEEEEEE }) };
		Sampler_Button plusButton = Sampler_Button("add-square"), minusButton = Sampler_Button("subtract-square");
		std::map<Geo_Actor*, Sampler_Button*> button_map;
#endif
	} _overlays;

	void createOverlays(double size);
	void _overlayCallback(MOUSE_Event event, Geo_Actor* actor);
};
