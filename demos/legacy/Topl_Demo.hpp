#include "Platform.hpp"

#include "Topl_Factory.hpp"
#include "Topl_Timeline.hpp"

#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"
#include "constructs/Geo_Billboards.hpp"

#include "legacy/All_Pipelines.h"

#define NO_PICKER_OBJ nullptr
#define CACHED_FRAME_COUNT 60
#define PROGRAM_M 0.1f
#define PROGRAM_SCENE Topl_Scene(&Topl_Demo::camera)
#define PROGRAM_PIPELINES 8
#define PROGRAM_BK_TESS 3
#define PROGRAM_BK_SIZE 5.0F
#define PROGRAM_BILLBOARDS 8
#define PROGRAM_SUBMENUS 9
#define PROGRAM_IS_BK true
#define PROGRAM_IS_OVERLAY true
#define PROGRAM_AUDIO_FRAMES 4096

class Topl_Demo {
public:
#ifndef __ANDROID__
#ifdef _WIN32
	Topl_Demo(const char* execPath, const char* name) : _backend(BACKEND_DX11) { setup(execPath, name); } // Windows setup
#else
    Topl_Demo(const char* execPath, const char* name) : _backend(BACKEND_GL4) { setup(execPath, name); } // Linux setup
#endif
	Topl_Demo(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) { setup(execPath, name); } // Custom Setup
#else
	Topl_Demo(android_app* app)  : _backend(BACKEND_GL4){ setup(app); } // Android setup
#endif
	~Topl_Demo(){ cleanup(); }

	void run();
#ifdef TOPL_ENABLE_AUDIO
	/* void menuSelect(unsigned short index);
	void play(std::string audioPathStr); // TODO: Add number of repitions
	ma_engine audioEngine; // for playback
	ma_decoder audioDecoder; // for analysis
	std::vector<float> audioData; // for capture
	kiss_fftr_cfg fftConfig = kiss_fftr_alloc(PROGRAM_AUDIO_FRAMES, false, NULL, NULL);
	kiss_fft_cpx fftOutput[PROGRAM_AUDIO_FRAMES]; */
#endif
#ifdef TOPL_ENABLE_TEXTURES
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
#ifdef TOPL_ENABLE_TEXTURES
	static Rasteron_Queue* cachedFrames; // frame capture queue
	// std::map<Geo_Actor*, Topl_Sampler_Array> sequence_map; // for dynamically changing textures
#endif
	std::map<Geo_Actor*, Vec3f> positions_map, rotations_map, scales_map; // for dynamic orientation changes
    bool isEnable_screencap = true, isEnable_console = true;
protected:
    virtual void init() = 0;
	virtual void preloop();
    virtual void loop(millisec_t frameTime) = 0;
	virtual void postloop();

	void getInput();
	void cleanup();

	// Rendering
	const enum BACKEND_Target _backend;
	Topl_Renderer* _renderer = nullptr;
	Platform* _platform;

	// Shaders and Pipelines

	Textured_VertexShader _texVShader; Textured_PixelShader _texPShader;
	Beams_VertexShader _beamsVShader; Beams_PixelShader _beamsPShader;
	Spectral_VertexShader _materialVShader; Spectral_PixelShader _materialPShader;
	Colorcode_VertexShader _coloredVShader; Colorcode_PixelShader _coloredPShader;
	Effect_VertexShader _effectVShader; Effect_PixelShader _effectPShader;
	Draw_VertexShader _drawVShader; Draw_PixelShader _drawPShader;
	Field_VertexShader _fieldVShader; Field_PixelShader _fieldPShader;
	Pattern_VertexShader _patternVShader; Pattern_PixelShader _patternPShader;
	Advance_GeometryShader _geomShaders[PROGRAM_PIPELINES]; Advance_TessCtrlShader _tessCtrlShaders[PROGRAM_PIPELINES]; Advance_TessEvalShader _tessEvalShaders[PROGRAM_PIPELINES];

	Topl_EntryShader* _entryShaders[PROGRAM_PIPELINES] = { &_texVShader, &_beamsVShader, &_materialVShader, &_coloredVShader, &_effectVShader, &_drawVShader, &_fieldVShader, &_patternVShader };
	Topl_Pipeline *_texPipeline, *_beamsPipeline, *_materialPipeline, *_coloredPipeline, *_effectPipeline, *_drawPipeline, *_fieldPipeline, *_patternPipeline; // for easy reuse
	Topl_Pipeline *_geomPipeline, *_tessPipeline, *_longPipeline;

    // Options & Properties
	enum PROGRAM_Menu { PROGRAM_Media = 0, PROGRAM_Object = 1, PROGRAM_Params = 2, PROGRAM_Timeline = 3, PROGRAM_Scene = 4, PROGRAM_Shaders = 5, PROGRAM_Sculpt = 6, PROGRAM_Paint = 7 };
	PROGRAM_Menu menuMode = PROGRAM_Shaders;

    void renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, int mode);
	void renderScene(Topl_Scene* scene){ renderScene(scene, nullptr, shaderMode); }
	virtual void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex){ std::cout << "Pane " << std::to_string((int)menu) << " pressed, index " << std::to_string(paneIndex) << std::endl; }

	bool isEnable_background = PROGRAM_IS_BK;
	Topl_Scene* getBackgroundScene() { return (isEnable_background) ? &_background.scene : nullptr; }
	bool isEnable_overlays = PROGRAM_IS_OVERLAY;
	Topl_Scene* getOverlaysScene() { return (isEnable_overlays)? &_overlays.scene : nullptr; }
private:
	static Topl_Pipeline* _savedPipeline;
	std::thread backgroundThread;

    void _onAnyKey(keyboard_t k);
    void _onAnyPress(enum MOUSE_Event event, std::pair<float, float> cursor);

#ifndef __ANDROID__
	void setup(const char* execPath, const char* name);
#else
	void setup(android_app* app);
#endif
	void setShadersMode(unsigned m) { for (unsigned s = 0; s < PROGRAM_PIPELINES; s++) _entryShaders[s]->setMode(m); }
    void setPipelines();
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
#ifdef TOPL_ENABLE_TEXTURES
        Topl_Sampler_2D image = Topl_Sampler_Gradient(SIDE_Radial, 0xFF111111, 0xFFEEEEEE); // = Topl_Sampler_File(std::string(IMAGES_DIR) + "Background-Action.bmp");
		Topl_Sampler_3D volume = Topl_Sampler_3D(256); // Topl_Sampler_3D(SAMPLER_WIDTH, SAMPLER_HEIGHT, 32);
#endif
	} _background;

	void createBackground(Topl_Sampler_2D* backgroundTex);
	void _backgroundCallback(MOUSE_Event event, Geo_Actor* actor);

	struct Editor {
		Geo_Quad3D mesh = Geo_Quad3D(1.0F);
		Geo_Actor actor = Geo_Actor("pickerEdit", &mesh);
#ifdef TOPL_ENABLE_TEXTURES
		std::string fontPath = std::string(FONTS_DIR) + "CutiveMono-Regular.ttf";
#if RASTERON_ENABLE_FONT
		Topl_Sampler_Text nameImg = Topl_Sampler_Text({ fontPath.c_str(), /* actor.getName().c_str() */ "1", 0xFF111111, 0xFFEEEEEE });
#else
        Topl_Sampler_Text nameImg = Topl_Sampler_Text();
#endif
        Geo_Quad2D nameMesh = Geo_Quad2D(0.05F);
		Geo_Actor nameActor = Geo_Actor("pickerName", &nameMesh);
		Topl_Scene scene = Topl_Scene(&Topl_Demo::camera, { &actor, &nameActor });
#else
		Topl_Scene scene = Topl_Scene(&Topl_Demo::camera, { &actor });
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
		Geo_Crossboard billboard_media = Geo_Crossboard("program_media", 3, &scene);
		Geo_Billboard billboard_object = Geo_Billboard("program_object", 5, 3, &scene);
		Geo_Billboard billboard_shader = Geo_Billboard("program_shader", 3, 3, &scene);
		Geo_Billboard* billboards[PROGRAM_BILLBOARDS] = { &billboard_media, &billboard_object, &billboard_shader, &billboard_timeline, &billboard_camera, &billboard_appbar, &billboard_sculpt, &billboard_paint, };
#if defined(RASTERON_H) && PROGRAM_IS_OVERLAY
		std::string fontPath = std::string(FONTS_DIR) + "Raleway-Regular.ttf";
		Topl_Sampler_2D textures[9]; // for switching textures
		Topl_Sampler_Button button = Topl_Sampler_Button();
        Topl_Sampler_Dial dials[3] = { Topl_Sampler_Dial(4), Topl_Sampler_Dial(4), Topl_Sampler_Dial(4) };
        Topl_Sampler_Slider timeSlider = Topl_Sampler_Slider(MENU_XL, (unsigned)TIMELINE_END); // sliders[1] = { Topl_Sampler_Slider(2) };
		Topl_Sampler_Slider sizeSliders[3] = { Topl_Sampler_Slider(3), Topl_Sampler_Slider(3), Topl_Sampler_Slider(3) };
		// std::pair<Topl_Sampler_Button, Topl_Sampler_Button> plusMinusButtons = std::make_pair(Topl_Sampler_Button("add-square"), Topl_Sampler_Button("subtract-square"));
		Topl_Sampler_Button numberButtons[PROGRAM_SUBMENUS] = {
			Topl_Sampler_Button("paginate-filter-1"), Topl_Sampler_Button("paginate-filter-2"), Topl_Sampler_Button("paginate-filter-3"),
			Topl_Sampler_Button("paginate-filter-4"), Topl_Sampler_Button("paginate-filter-5"), Topl_Sampler_Button("paginate-filter-6"),
			Topl_Sampler_Button("paginate-filter-7"), Topl_Sampler_Button("paginate-filter-8"), Topl_Sampler_Button("paginate-filter-9")
			// Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button(), Topl_Sampler_Button()
		};
		Topl_Sampler_Button pipelineButtons[PROGRAM_SUBMENUS] = {
			/*Topl_Sampler_Button("app-window"), Topl_Sampler_Button("app-window-edit"), Topl_Sampler_Button("app-window-module"),
			Topl_Sampler_Button("app-window-flash"), Topl_Sampler_Button("app-window-star-1"), Topl_Sampler_Button("app-window-next"),
			Topl_Sampler_Button("app-window-flag"), Topl_Sampler_Button("app-window-settings"), Topl_Sampler_Button("app-window-clock") */
			Topl_Sampler_Button("paginate-filter-1"), Topl_Sampler_Button("paginate-filter-2"), Topl_Sampler_Button("paginate-filter-3"),
			Topl_Sampler_Button("paginate-filter-4"), Topl_Sampler_Button("paginate-filter-5"), Topl_Sampler_Button("paginate-filter-6"),
			Topl_Sampler_Button("paginate-filter-7"), Topl_Sampler_Button("paginate-filter-8"), Topl_Sampler_Button("paginate-filter-9")
		};
		Topl_Sampler_Button sculptButtons[PROGRAM_SUBMENUS] = {
			/* Topl_Sampler_Button("tools-hammer"), Topl_Sampler_Button("tools-hammer-1"), Topl_Sampler_Button("tools-hammer-3"),
			Topl_Sampler_Button("tools-palette-trowel"), Topl_Sampler_Button("tools-flattener-triangle"), Topl_Sampler_Button("tools-saw"),
			Topl_Sampler_Button("tools-pliers"), Topl_Sampler_Button("tools-vice-grip"), Topl_Sampler_Button("tools-nail-gun") */
			Topl_Sampler_Button("paginate-filter-1"), Topl_Sampler_Button("paginate-filter-2"), Topl_Sampler_Button("paginate-filter-3"),
			Topl_Sampler_Button("paginate-filter-4"), Topl_Sampler_Button("paginate-filter-5"), Topl_Sampler_Button("paginate-filter-6"),
			Topl_Sampler_Button("paginate-filter-7"), Topl_Sampler_Button("paginate-filter-8"), Topl_Sampler_Button("paginate-filter-9")
		};
		Topl_Sampler_Button paintButtons[PROGRAM_SUBMENUS] = {
			/* Topl_Sampler_Button("content-pen"), Topl_Sampler_Button("content-pen-1"), Topl_Sampler_Button("content-pen-3"),
			Topl_Sampler_Button("content-pen-6"), Topl_Sampler_Button("content-ink-pen"), Topl_Sampler_Button("design-tool-quill-1"),
			Topl_Sampler_Button("color-rolling-brush"), Topl_Sampler_Button("design-tool-paper-glue"), Topl_Sampler_Button("color-bucket-brush") */
			Topl_Sampler_Button("paginate-filter-1"), Topl_Sampler_Button("paginate-filter-2"), Topl_Sampler_Button("paginate-filter-3"),
			Topl_Sampler_Button("paginate-filter-4"), Topl_Sampler_Button("paginate-filter-5"), Topl_Sampler_Button("paginate-filter-6"),
			Topl_Sampler_Button("paginate-filter-7"), Topl_Sampler_Button("paginate-filter-8"), Topl_Sampler_Button("paginate-filter-9")
		};
		Topl_Sampler_Button cameraButtons[3] = { Topl_Sampler_Button(/* "camera-retro" */), Topl_Sampler_Button(/* "camera-1" */), Topl_Sampler_Button(/* "camera-2" */) };
        Topl_Sampler_Button mediaButtons[3] = { Topl_Sampler_Button(/* "controls-forward" */), Topl_Sampler_Button(/* "controls-next" */), Topl_Sampler_Button(/* "controls-rewind" */) };
#if RASTERON_ENABLE_FONT
		Topl_Sampler_Label mediaLabels[3] = { Topl_Sampler_Label({ fontPath.c_str(), "|00|", 0xFF111111, 0xFFEEEEEE }), Topl_Sampler_Label({ fontPath.c_str(), "|00|", 0xFF111111, 0xFFEEEEEE }), Topl_Sampler_Label({ fontPath.c_str(), "|00|", 0xFF111111, 0xFFEEEEEE }) };
		// Topl_Sampler_Label timeLabel = Topl_Sampler_Label({ fontPath.c_str(), "0:00:00", 0xFF111111, 0xFFEEEEEE });
		Topl_Sampler_Label recordLabel = Topl_Sampler_Label({ fontPath.c_str(), "RECORD", 0xFF111111, 0xFFEEEEEE });
		Topl_Sampler_Label axisLabels[3] = { Topl_Sampler_Label({ fontPath.c_str(), "| X |", 0xFF333333, 0xFFEEEEEE }), Topl_Sampler_Label({ fontPath.c_str(), "| Y |", 0xFF333333, 0xFFEEEEEE }), Topl_Sampler_Label({ fontPath.c_str(), "| Z |", 0xFF333333, 0xFFEEEEEE }) };
#else
		Topl_Sampler_Label mediaLabels[3] = { Topl_Sampler_Label(), Topl_Sampler_Label(), Topl_Sampler_Label() }; Topl_Sampler_Label recordLabel = Topl_Sampler_Label(); Topl_Sampler_Label axisLabels[3] = { Topl_Sampler_Label(), Topl_Sampler_Label(), Topl_Sampler_Label() };
#endif
		Topl_Sampler_Button plusButton = Topl_Sampler_Button(/* "add-square" */), minusButton = Topl_Sampler_Button(/* "subtract-square" */);
		std::map<Geo_Actor*, Topl_Sampler_Button*> button_map;
#endif
	} _overlays;

	void createOverlays(double size);
	void _overlayCallback(MOUSE_Event event, Geo_Actor* actor);
};
