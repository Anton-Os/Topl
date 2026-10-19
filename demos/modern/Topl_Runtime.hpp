#ifndef TOPL_RUNTIME_H
#define TOPL_RUNTIME_H

#include "Platform.hpp"

#include "Topl_Factory.hpp"

#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Volume.hpp"
#include "meshes/Geo_Orboid.hpp"
#include "meshes/Geo_Cone.hpp"
#include "constructs/Geo_Billboards.hpp"

#include "legacy/color/Color_Shader.hpp" // picking for the menu panes
#include "legacy/textured/Textured_Shader.hpp" // drawing for the menu panes
#include "modern/Modern_Pipelines.h"

#include "resource.h" // ids from the window menu bar

#ifdef TOPL_ENABLE_AUDIO
	#include <miniaudio/miniaudio.h> // Audio Library
#endif

// Runtime shared by the modern demos: window, backend, modern pipelines, FPS camera, frame loop and the menu panes
// Menu panes follow the legacy demos: meshes on the left, textures on the right, camera and object properties
// on the top, and audio, sequencing and playback on the bottom

#define RUNTIME_FRAME_MAX 0.1 // longest frame step in seconds, avoids jumps after stalls
#define RUNTIME_MODES 4 // every modern shader exposes 4 modes
#define RUNTIME_ENVIRONMENTS 4
#define RUNTIME_PANE_ITEMS 9 // buttons per menu pane
#define RUNTIME_MESHES 9
#define RUNTIME_TEXTURES 9
#define RUNTIME_COLORS 8
#define RUNTIME_MATERIALS 4
#define RUNTIME_TRACKS 9
#define RUNTIME_TIMELINE_LENGTH 60.0 // seconds covered by the timeline slider
#define RUNTIME_TIMELINE_STEPS 60 // slider positions
#define RUNTIME_AUDIO_WINDOW 2048 // samples analyzed per frame

enum MODERN_Pipeline {
	MODERN_Sky = 0,
	MODERN_Lighting = 1,
	MODERN_Toon = 2,
	MODERN_Hologram = 3,
	MODERN_Terrain = 4,
	MODERN_Raymarch = 5,
	MODERN_Spacetime = 6,
	MODERN_Spectrum = 7,
	MODERN_Picker = 8,
	MODERN_PIPELINE_COUNT = 9
};

enum RUNTIME_Key {
	KEY_Forward, KEY_Back, KEY_Left, KEY_Right, KEY_Down, KEY_Up, KEY_Jump, KEY_Sprint,
	KEY_LookLeft, KEY_LookRight, KEY_LookUp, KEY_LookDown,
	KEY_ModeNext, KEY_ModePrev, KEY_Reset, KEY_Fly, KEY_ZoomIn, KEY_ZoomOut, KEY_Pause, KEY_Environment, KEY_Help, KEY_Overlay,
	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	KEY_COUNT
};

enum RUNTIME_Pane { // menu panes and the actions they hold
	PANE_Camera = 0, // top row: field of view, zoom, fly or walk, move speed, environment, reset
	PANE_Object = 1, // second top row: size, height, spin, color, material, reset and deselect for the selected object
	PANE_Meshes = 2, // left column: mesh of the selected object
	PANE_Textures = 3, // right column: texture of the selected object
	PANE_Media = 4, // bottom row: playback, speed, camera sequence and audio tracks
	PANE_Timeline = 5, // bottom slider: scrubs time
	PANE_COUNT = 6
};

// Vector helpers, Vec3f::normalize() keeps the engine's own scaling so demos normalize with these instead

inline float runtimeDot(const Vec3f& a, const Vec3f& b){ return a.data[0] * b.data[0] + a.data[1] * b.data[1] + a.data[2] * b.data[2]; }
inline Vec3f runtimeCross(const Vec3f& a, const Vec3f& b){
	return Vec3f({ a.data[1] * b.data[2] - a.data[2] * b.data[1], a.data[2] * b.data[0] - a.data[0] * b.data[2], a.data[0] * b.data[1] - a.data[1] * b.data[0] });
}
inline Vec3f runtimeNormalize(const Vec3f& v){ float len = v.len(); return (len > 1e-8F)? v * (1.0F / len) : Vec3f({ 0.0F, 1.0F, 0.0F }); }
inline Vec3f runtimeRotate(const Vec3f& v, const Vec3f& axis, float angle){ // rodrigues rotation around a unit axis
	float c = (float)cos(angle), s = (float)sin(angle);
	return v * c + runtimeCross(axis, v) * s + axis * (runtimeDot(axis, v) * (1.0F - c));
}

// Input polled directly from the OS each frame so held keys move the camera smoothly

class Runtime_Input {
public:
	void poll(NATIVE_PLATFORM_CONTEXT* context);
	bool isDown(RUNTIME_Key key) const { return _down[key]; }
	bool isPressed(RUNTIME_Key key) const { return _down[key] && !_prev[key]; } // true on the first frame of a press
	bool isMouseHeld() const { return _isMouseHeld; }
	bool isMousePressed() const { return _isMouseHeld && !_wasMouseHeld; } // true on the first frame of a click
	float getMouseDX() const { return _mouseDX; } // pixels moved since last poll while a button is held
	float getMouseDY() const { return _mouseDY; }
	float getAspect() const { return _aspect; } // window width over height
private:
	std::bitset<KEY_COUNT> _down, _prev;
	bool _isMouseHeld = false, _wasMouseHeld = false;
	int _mouseX = 0, _mouseY = 0;
	float _mouseDX = 0.0F, _mouseDY = 0.0F;
	float _aspect = (float)TOPL_WIN_WIDTH / (float)TOPL_WIN_HEIGHT;
};

// First person camera controller, angles are sent to the vertex shaders as yaw, pitch and roll

typedef std::function<float(float, float)> groundFunc; // returns ground height at an x and z position

struct Runtime_FPSCamera {
	void reset(Vec3f pos, float yawAngle, float pitchAngle);
	void update(const Runtime_Input& input, double frameSecs, bool isMouseLook);
	void apply(Topl_Camera* camera) const;

	Vec3f getForward() const; // full look direction
	Vec3f getFlatForward() const; // look direction projected on the ground
	Vec3f getRight() const;

	Vec3f position = Vec3f({ 0.0F, 1.7F, -6.0F });
	Vec3f velocity = Vec3f({ 0.0F, 0.0F, 0.0F });
	float yaw = 0.0F, pitch = 0.0F, roll = 0.0F, zoom = 1.0F;

	float moveSpeed = 6.0F; // units per second
	float sprintFactor = 2.5F;
	float lookSpeed = 1.8F; // radians per second with arrow keys
	float mouseSensitivity = 0.0035F; // radians per pixel
	float acceleration = 12.0F; // how fast velocity reaches its target
	float eyeHeight = 1.7F;
	float gravity = 18.0F;
	float jumpSpeed = 6.5F;
	bool isFlying = true; // flying moves along the look direction, walking stays on the ground
	groundFunc ground = nullptr; // optional ground used when walking
private:
	bool _isGrounded = false;
};

// Camera keyframes recorded on the timeline and replayed in a loop

struct Runtime_Keyframe {
	double time;
	Vec3f pos;
	float yaw, pitch;
};

class Runtime_Sequence {
public:
	void record(double time, const Runtime_FPSCamera& camera);
	bool sample(double time, Runtime_FPSCamera* camera) const; // moves the camera along the recorded path
	void clear(){ _keys.clear(); isPlaying = false; }
	unsigned getCount() const { return (unsigned)_keys.size(); }

	bool isPlaying = false;
private:
	std::vector<Runtime_Keyframe> _keys; // sorted by time
};

// Audio playback and spectrum analysis with miniaudio, inactive without SUPPORT_AUDIO

class Runtime_Audio {
public:
	~Runtime_Audio();

	bool play(unsigned track); // plays one of the test tones in the audio assets
	void stop();
	void setPaused(bool isPaused);
	bool isPlaying() const;
	bool isAvailable() const;
	unsigned getTrack() const { return _track; }
	static const char* getTrackName(unsigned track);

	bool analyze(float* levels, unsigned count, float minFreq, float maxFreq); // band levels from 0 to 1, false without live audio
private:
	static float goertzel(const float* samples, unsigned count, float freq, float sampleRate);

	unsigned _track = 0;
	bool _isPaused = false;
	std::vector<float> _window = std::vector<float>(RUNTIME_AUDIO_WINDOW, 0.0F);
#ifdef TOPL_ENABLE_AUDIO
	ma_engine _engine;
	ma_sound _sound;
	ma_decoder _decoder;
	bool _isEngine = false, _isSound = false, _isDecoder = false;
	float _sampleRate = 44100.0F;
#endif
};

// Flat grid on the XZ plane with even triangles, used for terrain, fabric and floors

class Runtime_Grid : public Geo_Mesh {
public:
	Runtime_Grid(float size, unsigned short divisions);

	float getSize() const { return _size; }
	float getSpacing() const { return _size / _divisions; }
private:
	float _size;
	unsigned short _divisions;
};

// Meshes offered in the left pane

struct Runtime_MeshLibrary {
	Runtime_MeshLibrary();

	Geo_Mesh* getMesh(unsigned index){ return meshes[index % RUNTIME_MESHES]; }
	float getNormScale(unsigned index) const { return normScales[index % RUNTIME_MESHES]; } // evens out mesh sizes
	bool isSmooth(unsigned index) const { return index % RUNTIME_MESHES == 0; } // only the orb uses sphere normals
	static const char* getName(unsigned index);

	Geo_Orb orb = Geo_Orb({ 1.0F, 32, 32 });
	Geo_Quad3D cube = Geo_Quad3D(1.0F);
	Geo_Torus torus = Geo_Torus(0.5F);
	Geo_Hex3D hexPrism = Geo_Hex3D(1.0F);
	Geo_Circle3D cylinder = Geo_Circle3D(0.8F);
	Geo_Trig3D triPrism = Geo_Trig3D(1.0F);
	Geo_CircleCone cone = Geo_CircleCone(1.0F);
	Geo_QuadCone pyramid = Geo_QuadCone(1.0F);
	Geo_Orb gem = Geo_Orb({ 1.0F, 6, 4 });

	Geo_Mesh* meshes[RUNTIME_MESHES] = { &orb, &cube, &torus, &hexPrism, &cylinder, &triPrism, &cone, &pyramid, &gem };
	float normScales[RUNTIME_MESHES];
};

// Object edits from the menu panes, applied while an actor is drawn so demos keep animating freely

struct Runtime_Edit {
	float scale = 1.0F;
	float lift = 0.0F;
	bool isSpinning = false;
	int mesh = -1, color = -1, material = -1, texture = -1; // -1 keeps the demo's own choice
};

#ifdef TOPL_ENABLE_TEXTURES
// Menu panes built from legacy billboards, each pane lives in its own scene so refreshing one pane stays cheap

struct Runtime_Overlays {
	Runtime_Overlays();
	~Runtime_Overlays();

	Geo_Billboard* getBoard(RUNTIME_Pane pane){ return boards[pane]; }
	bool isReversed(RUNTIME_Pane pane) const { return pane == PANE_Camera || pane == PANE_Object || pane == PANE_Media; } // rows count right to left

	Topl_Camera camera; // identity camera keeps the panes in screen space
	Topl_Scene scenes[PANE_COUNT] = { Topl_Scene(&camera), Topl_Scene(&camera), Topl_Scene(&camera), Topl_Scene(&camera), Topl_Scene(&camera), Topl_Scene(&camera) };
	Geo_Crossboard cameraRow = Geo_Crossboard("runtime_camera", RUNTIME_PANE_ITEMS, &scenes[PANE_Camera]);
	Geo_Crossboard objectRow = Geo_Crossboard("runtime_object", RUNTIME_PANE_ITEMS, &scenes[PANE_Object]);
	Geo_Listboard meshList = Geo_Listboard("runtime_meshes", RUNTIME_PANE_ITEMS, &scenes[PANE_Meshes]);
	Geo_Listboard textureList = Geo_Listboard("runtime_textures", RUNTIME_PANE_ITEMS, &scenes[PANE_Textures]);
	Geo_Crossboard mediaRow = Geo_Crossboard("runtime_media", RUNTIME_PANE_ITEMS, &scenes[PANE_Media]);
	Geo_Paneboard timeline = Geo_Paneboard("runtime_timeline", &scenes[PANE_Timeline]);
	Geo_Billboard* boards[PANE_COUNT] = { &cameraRow, &objectRow, &meshList, &textureList, &mediaRow, &timeline };

	Topl_Sampler_Button* buttons[PANE_Timeline][RUNTIME_PANE_ITEMS]; // numbered buttons for every pane except the timeline
	Topl_Sampler_Slider timeSlider = Topl_Sampler_Slider(MENU_XL, RUNTIME_TIMELINE_STEPS);
	int sliderStep = -1;
};
#endif

// Runtime

class Topl_Runtime {
public:
	Topl_Runtime(const char* execPath, const char* name) : Topl_Runtime(execPath, name, TOPL_DEFAULT_BACKEND){ }
	Topl_Runtime(const char* execPath, const char* name, BACKEND_Target backend);
	virtual ~Topl_Runtime();

	void run();

	static BACKEND_Target getBackendArg(int argc, char** argv); // --gl, --dx and --vk override the platform default
	static const char* getBackendName(BACKEND_Target backend);
	BACKEND_Target getBackend() const { return _backend; }
protected:
	virtual void init() = 0; // build scenes here
	virtual void loop(double frameSecs) = 0; // update and render here, the frame is cleared and presented by the runtime
	virtual void onPress(RUNTIME_Key key){ } // invoked for each key pressed this frame
	virtual void onPipelineSelect(unsigned index){ shaderMode = index % RUNTIME_MODES; } // Pipelines entries of the window menu
	virtual void onMeshChanged(Geo_Actor* actor){ } // after the left pane swaps an actor's mesh
	virtual bool isPickEnabled() const { return true; } // false while editable objects are hidden
	virtual bool isMaterialEditable() const { return true; } // false when a demo uses actor params for its own data
	virtual void printControls() const;

	void buildScene(Topl_Scene* scene);
	void renderScene(Topl_Scene* scene, MODERN_Pipeline pipeline, int mode, DRAW_Mode drawMode = DRAW_Triangles);
	void renderScene(Topl_Scene* scene, Topl_Pipeline* pipeline, Modern_VertexShader* vertexShader, int mode, DRAW_Mode drawMode = DRAW_Triangles); // demo owned pipeline
	void renderSky(); // fullscreen sky matching the current environment
	void renderFullscreen(MODERN_Pipeline pipeline, int mode); // fullscreen pass such as the raymarcher
	void renderFullscreen(Topl_Pipeline* pipeline, Modern_VertexShader* vertexShader, int mode);

	// Demo owned pipelines: assign the _GL4 or _DX11 shader classes based on isGLSL(), then generate inside init()
	bool isGLSL() const; // true when the backend compiles the glsl shader sources
	Topl_Pipeline* genPipeline(Modern_VertexShader* vertexShader, Modern_PixelShader* pixelShader);
	void setEnvironment(unsigned index); // 0 day, 1 cloudy, 2 night, 3 aurora
	unsigned getEnvironment() const { return _environment; }

	// Menu targets: editable actors can be picked in the world and changed from the menu panes
	void addEditable(Geo_Actor* actor, Topl_Scene* scene);
	Geo_Actor* getSelected() const { return _selected; }
	void select(Geo_Actor* actor);
	void selectMesh(unsigned index); // left pane
	void selectTexture(unsigned index); // right pane
	void onCameraItem(unsigned index); // top row
	void onObjectItem(unsigned index); // second top row
	void onMediaItem(unsigned index); // bottom row
	void menuSelect(unsigned short menuID); // window menu bar
	float getMeshHeight(const Geo_Mesh* mesh); // vertical extent of a mesh, cached
	double getTimelineTime() const; // position inside the looping timeline

	Modern_VertexShader* getVertexShader(MODERN_Pipeline pipeline){ return _vertexShaders[pipeline]; }
	static const char* getPipelineName(MODERN_Pipeline pipeline);

	BACKEND_Target _backend;
	Platform* _platform = nullptr;
	Topl_Renderer* _renderer = nullptr;

	Topl_Camera camera;
	Runtime_FPSCamera fps;
	Runtime_Input input;
	Runtime_Audio audio;
	Runtime_Sequence sequence;
	Runtime_MeshLibrary meshLibrary;

	double elapsed = 0.0; // animation time in seconds, stops while paused
	double timeScale = 1.0; // playback speed from the bottom pane
	unsigned long frames = 0;
	bool isPaused = false;
	bool isOverlays = true; // menu panes are shown, toggled with O
	int shaderMode = 0; // changed with M and N, demos decide which pipelines receive it
	float fov = MODERN_FOV;

	// Shaders and Pipelines

	Sky_VertexShader _skyVShader; Sky_PixelShader _skyPShader;
	Lighting_VertexShader _lightingVShader; Lighting_PixelShader _lightingPShader;
	Toon_VertexShader _toonVShader; Toon_PixelShader _toonPShader;
	Hologram_VertexShader _hologramVShader; Hologram_PixelShader _hologramPShader;
	Terrain_VertexShader _terrainVShader; Terrain_PixelShader _terrainPShader;
	Raymarch_VertexShader _raymarchVShader; Raymarch_PixelShader _raymarchPShader;
	Spacetime_VertexShader _spacetimeVShader; Spacetime_PixelShader _spacetimePShader;
	Spectrum_VertexShader _spectrumVShader; Spectrum_PixelShader _spectrumPShader;
	Picker_VertexShader _pickerVShader; Picker_PixelShader _pickerPShader;

	Modern_VertexShader* _vertexShaders[MODERN_PIPELINE_COUNT] = {
		&_skyVShader, &_lightingVShader, &_toonVShader, &_hologramVShader, &_terrainVShader, &_raymarchVShader, &_spacetimeVShader, &_spectrumVShader, &_pickerVShader
	};
	Modern_PixelShader* _pixelShaders[MODERN_PIPELINE_COUNT] = {
		&_skyPShader, &_lightingPShader, &_toonPShader, &_hologramPShader, &_terrainPShader, &_raymarchPShader, &_spacetimePShader, &_spectrumPShader, &_pickerPShader
	};
	Topl_Pipeline* _pipelines[MODERN_PIPELINE_COUNT] = { nullptr };
private:
	void setup(const char* execPath, const char* name);
	void genPipelines();
	void updateFrame(double frameSecs);
	void handleKeys();
	void applyEdits(Geo_Actor* actor); // temporary changes made while drawing
	std::vector<Runtime_Edit*> getTargetEdits(std::vector<Geo_Actor*>* actors); // selected actor or every editable
	Topl_Scene* getEditableScene(const Geo_Actor* actor);
	void pickWorld();
#ifdef TOPL_ENABLE_TEXTURES
	void createOverlays();
	void renderOverlays();
	bool pickOverlays(); // true when a pane was clicked
	void scrubTimeline();
	void pressPane(RUNTIME_Pane pane, unsigned short cell);
	void genTexture(unsigned index);
#endif

	Timer_Persist _ticker;
	unsigned _environment = 0;
	Vec3f _startPos = Vec3f({ 0.0F, 1.7F, -6.0F });
	float _startYaw = 0.0F, _startPitch = 0.0F;

	// Editing
	std::vector<std::pair<Geo_Actor*, Topl_Scene*>> _editables;
	std::map<const Geo_Actor*, Runtime_Edit> _edits;
	std::map<const Geo_Mesh*, float> _meshHeights;
	Geo_Actor* _selected = nullptr;
	Topl_Scene _pickScene = Topl_Scene(&camera); // editable actors only, drawn by the picker pipeline
	bool _isPaneDrag = false, _isTimelineDrag = false;

	// fullscreen quad shared by sky and raymarch passes
	Geo_Quad2D _screenMesh = Geo_Quad2D(1.0F);
	Geo_Actor _screenActor = Geo_Actor("runtime_screen", &_screenMesh);
	Topl_Scene _screenScene = Topl_Scene(&camera, { &_screenActor });

#ifdef TOPL_ENABLE_TEXTURES
	Runtime_Overlays _overlays;
	Topl_Sampler_2D _textures[RUNTIME_TEXTURES];
	bool _isTextureMade[RUNTIME_TEXTURES] = { false };

	Color_VertexShader _colorVShader; Color_PixelShader _colorPShader;
	Textured_VertexShader _texVShader; Textured_PixelShader _texPShader;
	Topl_Pipeline* _colorPipeline = nullptr;
	Topl_Pipeline* _texPipeline = nullptr;
#endif
protected:
	void setStart(Vec3f pos, float yawAngle, float pitchAngle){ _startPos = pos; _startYaw = yawAngle; _startPitch = pitchAngle; fps.reset(pos, yawAngle, pitchAngle); }
};

#endif
