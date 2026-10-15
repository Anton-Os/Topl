#include "Topl_Runtime.hpp"

#include <new>

#if defined(__linux__) && !defined(__ANDROID__)
	#include <X11/keysym.h>
#endif

static const char* paneNames[PANE_COUNT] = { "Camera", "Object", "Meshes", "Textures", "Media", "Timeline" };
static const char* cameraItems[RUNTIME_PANE_ITEMS] = { "Narrower view", "Wider view", "Zoom in", "Zoom out", "Fly / walk", "Move slower", "Move faster", "Next environment", "Reset camera" };
static const char* objectItems[RUNTIME_PANE_ITEMS] = { "Smaller", "Larger", "Raise", "Lower", "Spin", "Next color", "Next material", "Reset object", "Deselect" };
static const char* textureItems[RUNTIME_TEXTURES] = { "No texture", "Checker", "Stripes", "Gradient", "Noise", "Octave noise", "Tiled noise", "Grid image", "Random image" };
static const char* mediaItems[RUNTIME_PANE_ITEMS] = { "Restart", "Play / pause", "Slower", "Faster", "Record camera key", "Play camera sequence", "Clear sequence", "Next audio track", "Stop audio" };
static const char* materialNames[RUNTIME_MATERIALS] = { "Matte", "Glossy", "Mirror", "Glow" };
static const Modern_Float4 materialParams[RUNTIME_MATERIALS] = { // shininess, reflectivity, glow
	Modern_Float4(8.0F, 0.0F, 0.0F, 0.0F), Modern_Float4(64.0F, 0.2F, 0.0F, 0.0F), Modern_Float4(128.0F, 0.9F, 0.0F, 0.0F), Modern_Float4(32.0F, 0.0F, 1.2F, 0.0F)
};
static const Modern_Float4 colorPalette[RUNTIME_COLORS] = {
	Modern_Float4(0.9F, 0.25F, 0.2F, 1.0F), Modern_Float4(1.0F, 0.6F, 0.2F, 1.0F), Modern_Float4(0.95F, 0.85F, 0.3F, 1.0F), Modern_Float4(0.35F, 0.85F, 0.4F, 1.0F),
	Modern_Float4(0.25F, 0.8F, 0.95F, 1.0F), Modern_Float4(0.3F, 0.45F, 0.95F, 1.0F), Modern_Float4(0.7F, 0.4F, 0.95F, 1.0F), Modern_Float4(0.92F, 0.92F, 0.92F, 1.0F)
};
static const char* audioTracks[RUNTIME_TRACKS] = {
	"40hz-sine-freqies.mp3", "80hz-sine-freqies.mp3", "150hz-sine-freqies.mp3",
	"300hz-sine-freqies.mp3", "500hz-sine-freqies.mp3", "800hz-sine-freqies.mp3",
	"1000hz-sine-freqies.mp3", "2000hz-sine-freqies.mp3", "5000hz-sine-freqies.mp3"
};

// Input

void Runtime_Input::poll(NATIVE_PLATFORM_CONTEXT* context){
	_prev = _down;
	_down.reset();
	_wasMouseHeld = _isMouseHeld;
	_mouseDX = 0.0F; _mouseDY = 0.0F;
	if(context == nullptr) return;

	bool isHeld = false;
	int mouseX = _mouseX, mouseY = _mouseY;
#if defined(_WIN32)
	static const int keyCodes[KEY_COUNT] = {
		'W', 'S', 'A', 'D', 'Q', 'E', VK_SPACE, VK_SHIFT,
		VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
		'M', 'N', 'R', 'F', 'Z', 'X', 'P', 'T', 'H', 'O',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	if(GetForegroundWindow() != context->window){ _isMouseHeld = false; return; } // ignore input while another window is focused

	for(unsigned k = 0; k < KEY_COUNT; k++) _down[k] = (GetAsyncKeyState(keyCodes[k]) & 0x8000) != 0;
	isHeld = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000);

	POINT point;
	if(GetCursorPos(&point) && ScreenToClient(context->window, &point)){ mouseX = point.x; mouseY = point.y; }

	RECT rect;
	if(GetClientRect(context->window, &rect) && rect.bottom - rect.top > 0)
		_aspect = (float)(rect.right - rect.left) / (float)(rect.bottom - rect.top);
#elif defined(__linux__) && !defined(__ANDROID__)
	static const KeySym keySyms[KEY_COUNT] = {
		XK_w, XK_s, XK_a, XK_d, XK_q, XK_e, XK_space, XK_Shift_L,
		XK_Left, XK_Right, XK_Up, XK_Down,
		XK_m, XK_n, XK_r, XK_f, XK_z, XK_x, XK_p, XK_t, XK_h, XK_o,
		XK_0, XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7, XK_8, XK_9
	};
	if(context->display == nullptr) return;

	char keymap[32];
	XQueryKeymap(context->display, keymap);
	for(unsigned k = 0; k < KEY_COUNT; k++){
		KeyCode code = XKeysymToKeycode(context->display, keySyms[k]);
		_down[k] = code != 0 && (keymap[code / 8] & (1 << (code % 8))) != 0;
	}

	Window rootWin, childWin;
	int rootX, rootY, winX, winY;
	unsigned buttonMask = 0;
	if(XQueryPointer(context->display, context->window, &rootWin, &childWin, &rootX, &rootY, &winX, &winY, &buttonMask)){
		mouseX = winX; mouseY = winY;
		isHeld = (buttonMask & (Button1Mask | Button3Mask)) != 0;
	}

	XWindowAttributes attribs;
	if(XGetWindowAttributes(context->display, context->window, &attribs) && attribs.height > 0)
		_aspect = (float)attribs.width / (float)attribs.height;
#endif
	if(isHeld && _isMouseHeld){ // only report movement while dragging
		_mouseDX = (float)(mouseX - _mouseX);
		_mouseDY = (float)(mouseY - _mouseY);
	}
	_isMouseHeld = isHeld;
	_mouseX = mouseX; _mouseY = mouseY;
}

// FPS Camera

void Runtime_FPSCamera::reset(Vec3f pos, float yawAngle, float pitchAngle){
	position = pos;
	velocity = Vec3f({ 0.0F, 0.0F, 0.0F });
	yaw = yawAngle; pitch = pitchAngle; roll = 0.0F; zoom = 1.0F;
}

Vec3f Runtime_FPSCamera::getForward() const { return Vec3f({ (float)(sin(yaw) * cos(pitch)), (float)sin(pitch), (float)(cos(yaw) * cos(pitch)) }); }

Vec3f Runtime_FPSCamera::getFlatForward() const { return Vec3f({ (float)sin(yaw), 0.0F, (float)cos(yaw) }); }

Vec3f Runtime_FPSCamera::getRight() const { return Vec3f({ (float)cos(yaw), 0.0F, (float)-sin(yaw) }); }

void Runtime_FPSCamera::update(const Runtime_Input& input, double frameSecs, bool isMouseLook){
	float dt = (float)frameSecs;

	// Looking
	float lookX = (input.isDown(KEY_LookRight)? 1.0F : 0.0F) - (input.isDown(KEY_LookLeft)? 1.0F : 0.0F);
	float lookY = (input.isDown(KEY_LookUp)? 1.0F : 0.0F) - (input.isDown(KEY_LookDown)? 1.0F : 0.0F);
	yaw += lookX * lookSpeed * dt;
	pitch += lookY * lookSpeed * dt;
	if(isMouseLook && input.isMouseHeld()){
		yaw += input.getMouseDX() * mouseSensitivity;
		pitch -= input.getMouseDY() * mouseSensitivity; // screen y grows downward
	}
	if(pitch > 1.5F) pitch = 1.5F;
	else if(pitch < -1.5F) pitch = -1.5F;
	if(yaw > MATH_PI * 2.0F) yaw -= MATH_PI * 2.0F;
	else if(yaw < -MATH_PI * 2.0F) yaw += MATH_PI * 2.0F;

	// Zooming
	if(input.isDown(KEY_ZoomIn)) zoom *= 1.0F + dt;
	if(input.isDown(KEY_ZoomOut)) zoom /= 1.0F + dt;
	if(zoom < 0.25F) zoom = 0.25F;
	else if(zoom > 6.0F) zoom = 6.0F;

	// Moving
	float f = (input.isDown(KEY_Forward)? 1.0F : 0.0F) - (input.isDown(KEY_Back)? 1.0F : 0.0F);
	float s = (input.isDown(KEY_Right)? 1.0F : 0.0F) - (input.isDown(KEY_Left)? 1.0F : 0.0F);
	float u = (input.isDown(KEY_Up)? 1.0F : 0.0F) - (input.isDown(KEY_Down)? 1.0F : 0.0F);
	if(isFlying && input.isDown(KEY_Jump)) u = 1.0F;

	Vec3f forward = (isFlying)? getForward() : getFlatForward();
	Vec3f right = getRight();
	Vec3f wish = (forward * f) + (right * s);
	if(isFlying) wish = wish + Vec3f({ 0.0F, u, 0.0F });
	float wishLen = wish.len();
	if(wishLen > 0.0001F) wish = wish * (1.0F / wishLen);

	float speed = moveSpeed * ((input.isDown(KEY_Sprint))? sprintFactor : 1.0F);
	float blend = (acceleration * dt < 1.0F)? acceleration * dt : 1.0F;
	velocity.data[0] += (wish.data[0] * speed - velocity.data[0]) * blend;
	velocity.data[2] += (wish.data[2] * speed - velocity.data[2]) * blend;
	if(isFlying) velocity.data[1] += (wish.data[1] * speed - velocity.data[1]) * blend;
	else {
		velocity.data[1] -= gravity * dt;
		if(_isGrounded && input.isDown(KEY_Jump)) velocity.data[1] = jumpSpeed;
	}

	position = position + (velocity * dt);

	// Ground collision
	float groundY = (ground != nullptr)? ground(position.data[0], position.data[2]) : 0.0F;
	if(!isFlying){
		_isGrounded = position.data[1] <= groundY + eyeHeight;
		if(_isGrounded){
			position.data[1] = groundY + eyeHeight;
			if(velocity.data[1] < 0.0F) velocity.data[1] = 0.0F;
		}
	} else if(ground != nullptr && position.data[1] < groundY + 0.5F) position.data[1] = groundY + 0.5F; // no flying underground
}

void Runtime_FPSCamera::apply(Topl_Camera* camera) const {
	camera->setPos(position);
	camera->setRot(Vec3f({ yaw, pitch, roll }));
	camera->setZoom(zoom);
}

// Sequence

void Runtime_Sequence::record(double time, const Runtime_FPSCamera& camera){
	Runtime_Keyframe key = { time, camera.position, camera.yaw, camera.pitch };
	auto next = _keys.begin();
	while(next != _keys.end() && next->time < time) next++;
	if(next != _keys.end() && fabs(next->time - time) < 0.05) *next = key; // replaces a key at the same moment
	else _keys.insert(next, key);
}

bool Runtime_Sequence::sample(double time, Runtime_FPSCamera* camera) const {
	if(_keys.empty() || camera == nullptr) return false;

	const Runtime_Keyframe* a = &_keys.front();
	const Runtime_Keyframe* b = &_keys.front();
	if(time >= _keys.back().time) a = b = &_keys.back();
	else if(time > _keys.front().time)
		for(unsigned k = 1; k < _keys.size(); k++)
			if(_keys[k].time >= time){ a = &_keys[k - 1]; b = &_keys[k]; break; }

	float t = (b->time > a->time)? (float)((time - a->time) / (b->time - a->time)) : 0.0F;
	t = t * t * (3.0F - 2.0F * t); // ease in and out between keys

	float yawDelta = b->yaw - a->yaw; // turn the short way around
	while(yawDelta > MATH_PI) yawDelta -= MATH_PI * 2.0F;
	while(yawDelta < -MATH_PI) yawDelta += MATH_PI * 2.0F;

	camera->position = a->pos + ((b->pos - a->pos) * t);
	camera->velocity = Vec3f({ 0.0F, 0.0F, 0.0F });
	camera->yaw = a->yaw + yawDelta * t;
	camera->pitch = a->pitch + (b->pitch - a->pitch) * t;
	return true;
}

// Audio

Runtime_Audio::~Runtime_Audio(){
	stop();
#ifdef TOPL_ENABLE_AUDIO
	if(_isEngine) ma_engine_uninit(&_engine);
#endif
}

const char* Runtime_Audio::getTrackName(unsigned track){ return audioTracks[track % RUNTIME_TRACKS]; }

bool Runtime_Audio::isAvailable() const {
#ifdef TOPL_ENABLE_AUDIO
	return true;
#else
	return false;
#endif
}

bool Runtime_Audio::isPlaying() const {
#ifdef TOPL_ENABLE_AUDIO
	return _isSound && !_isPaused;
#else
	return false;
#endif
}

bool Runtime_Audio::play(unsigned track){
#ifdef TOPL_ENABLE_AUDIO
	stop();
	if(!_isEngine) _isEngine = ma_engine_init(NULL, &_engine) == MA_SUCCESS;
	if(!_isEngine){ logMessage(MESSAGE_Exclaim, "Audio engine failed to initialize"); return false; }

	std::string path = std::string(AUDIO_DIR) + audioTracks[track % RUNTIME_TRACKS];
#ifdef _WIN32
	std::replace(path.begin(), path.end(), '/', '\\');
#endif
	_isSound = ma_sound_init_from_file(&_engine, path.c_str(), 0, NULL, NULL, &_sound) == MA_SUCCESS;
	if(!_isSound){ logMessage(MESSAGE_Exclaim, "Audio file failed to load"); return false; }
	ma_sound_set_looping(&_sound, MA_TRUE);
	ma_sound_start(&_sound);
	_isPaused = false;

	ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 0); // mono floats at the native sample rate
	_isDecoder = ma_decoder_init_file(path.c_str(), &config, &_decoder) == MA_SUCCESS;
	if(_isDecoder) _sampleRate = (float)_decoder.outputSampleRate;

	_track = track % RUNTIME_TRACKS;
	std::cout << "Playing " << audioTracks[_track] << std::endl;
	return true;
#else
	std::cout << "Audio playback requires SUPPORT_AUDIO" << std::endl;
	return false;
#endif
}

void Runtime_Audio::stop(){
#ifdef TOPL_ENABLE_AUDIO
	if(_isSound){ ma_sound_uninit(&_sound); _isSound = false; }
	if(_isDecoder){ ma_decoder_uninit(&_decoder); _isDecoder = false; }
#endif
	_isPaused = false;
}

void Runtime_Audio::setPaused(bool isPaused){
#ifdef TOPL_ENABLE_AUDIO
	if(_isSound && isPaused != _isPaused){
		if(isPaused) ma_sound_stop(&_sound);
		else ma_sound_start(&_sound);
	}
#endif
	_isPaused = isPaused;
}

float Runtime_Audio::goertzel(const float* samples, unsigned count, float freq, float sampleRate){
	float coeff = 2.0F * (float)cos(2.0 * MATH_PI * freq / sampleRate);
	float s1 = 0.0F, s2 = 0.0F;
	for(unsigned n = 0; n < count; n++){
		float window = 0.5F - 0.5F * (float)cos(2.0 * MATH_PI * n / (count - 1)); // hann window
		float s = samples[n] * window + coeff * s1 - s2;
		s2 = s1; s1 = s;
	}
	float power = s1 * s1 + s2 * s2 - coeff * s1 * s2;
	return (power > 0.0F)? (float)sqrt(power) / (count * 0.25F) : 0.0F; // amplitude normalized for the window
}

bool Runtime_Audio::analyze(float* levels, unsigned count, float minFreq, float maxFreq){
#ifdef TOPL_ENABLE_AUDIO
	if(!_isSound || !_isDecoder || _isPaused || count == 0) return false;

	ma_uint64 cursor = 0; // read the samples currently being heard
	ma_sound_get_cursor_in_pcm_frames(&_sound, &cursor);
	ma_uint64 start = (cursor > RUNTIME_AUDIO_WINDOW)? cursor - RUNTIME_AUDIO_WINDOW : 0;
	ma_uint64 framesRead = 0;
	if(ma_decoder_seek_to_pcm_frame(&_decoder, start) == MA_SUCCESS)
		ma_decoder_read_pcm_frames(&_decoder, _window.data(), RUNTIME_AUDIO_WINDOW, &framesRead);
	for(ma_uint64 s = framesRead; s < RUNTIME_AUDIO_WINDOW; s++) _window[(size_t)s] = 0.0F;

	for(unsigned b = 0; b < count; b++){
		float freq = minFreq * (float)pow(maxFreq / minFreq, (count > 1)? (float)b / (float)(count - 1) : 0.0F); // log spaced bands
		float amp = goertzel(_window.data(), RUNTIME_AUDIO_WINDOW, freq, _sampleRate);
		float level = (20.0F * (float)log10(amp + 1e-6F) + 60.0F) / 60.0F; // -60 dB to 0 dB maps to 0 to 1
		levels[b] = (level < 0.0F)? 0.0F : (level > 1.0F)? 1.0F : level;
	}
	return true;
#else
	return false;
#endif
}

// Grid

Runtime_Grid::Runtime_Grid(float size, unsigned short divisions) : Geo_Mesh() {
	_size = size;
	_divisions = (divisions > 0)? divisions : 1;

	unsigned row = _divisions + 1;
	unsigned vertexCount = row * row;
	unsigned indexCount = _divisions * _divisions * 6;
	float spacing = _size / _divisions;
	float half = _size * 0.5F;

	// populate() copies and then frees both arrays, so they are allocated with malloc
	Geo_Vertex* vertices = (Geo_Vertex*)malloc(sizeof(Geo_Vertex) * vertexCount);
	unsigned* indices = (unsigned*)malloc(sizeof(unsigned) * indexCount);

	for(unsigned z = 0; z < row; z++)
		for(unsigned x = 0; x < row; x++)
			new (&vertices[z * row + x]) Geo_Vertex(
				Vec3f({ -half + x * spacing, 0.0F, -half + z * spacing }), // position
				Vec3f({ (float)x / _divisions, (float)z / _divisions, 0.0F }), // texcoord
				Vec3f({ 0.0F, 1.0F, 0.0F }), // normal
				Vec3f({ 1.0F, 1.0F, 1.0F }) // color
			);

	unsigned i = 0;
	for(unsigned z = 0; z < _divisions; z++)
		for(unsigned x = 0; x < _divisions; x++){
			unsigned a = z * row + x, b = a + 1, c = a + row, d = c + 1;
			indices[i++] = a; indices[i++] = c; indices[i++] = b;
			indices[i++] = b; indices[i++] = c; indices[i++] = d;
		}

	populate(vertexCount, vertices, indexCount, indices);
}

// Mesh Library

Runtime_MeshLibrary::Runtime_MeshLibrary(){
	for(unsigned m = 0; m < RUNTIME_MESHES; m++){ // scale every mesh to the radius of the orb
		float radius = 0.0F;
		for(unsigned v = 0; v < meshes[m]->getVertexCount(); v++){
			float len = Vec3f(meshes[m]->getVertices()[v].position).len();
			if(len > radius) radius = len;
		}
		normScales[m] = (radius > 0.0001F)? 0.7071F / radius : 1.0F;
	}
}

const char* Runtime_MeshLibrary::getName(unsigned index){
	static const char* names[RUNTIME_MESHES] = { "Orb", "Cube", "Torus", "Hex prism", "Cylinder", "Triangle prism", "Cone", "Pyramid", "Gem" };
	return names[index % RUNTIME_MESHES];
}

#ifdef TOPL_ENABLE_TEXTURES
// Overlays

Runtime_Overlays::Runtime_Overlays(){
	for(unsigned p = 0; p < PANE_Timeline; p++)
		for(unsigned b = 0; b < RUNTIME_PANE_ITEMS; b++){
			std::string iconName = "paginate-filter-" + std::to_string(b + 1);
			buttons[p][b] = new Topl_Sampler_Button(&iconName[0]);
		}

	// rows read left to right and columns top to bottom, so the numbers match the legacy demos
	for(unsigned p = 0; p < PANE_Timeline; p++)
		for(unsigned b = 0; b < RUNTIME_PANE_ITEMS; b++)
			boards[p]->overlay(b, buttons[p][(isReversed((RUNTIME_Pane)p))? RUNTIME_PANE_ITEMS - 1 - b : b]);
	timeline.overlay(0, &timeSlider);
}

Runtime_Overlays::~Runtime_Overlays(){
	for(unsigned p = 0; p < PANE_Timeline; p++)
		for(unsigned b = 0; b < RUNTIME_PANE_ITEMS; b++) delete buttons[p][b];
}
#endif

// Runtime

Topl_Runtime::Topl_Runtime(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
	setup(execPath, name);
}

Topl_Runtime::~Topl_Runtime(){
	Modern_VertexShader::getHighlight() = nullptr;
	if(_renderer != nullptr) delete(_renderer);
	if(_platform != nullptr) delete(_platform);
}

BACKEND_Target Topl_Runtime::getBackendArg(int argc, char** argv){
	for(int a = 1; a < argc; a++){
		std::string arg = argv[a];
		if(arg == "--gl" || arg == "--opengl") return BACKEND_GL4;
#ifdef _WIN32
		if(arg == "--dx" || arg == "--directx") return BACKEND_DX11;
#endif
#ifdef TOPL_ENABLE_VULKAN
		if(arg == "--vk" || arg == "--vulkan") return BACKEND_VK;
#endif
	}
	return TOPL_DEFAULT_BACKEND;
}

const char* Topl_Runtime::getBackendName(BACKEND_Target backend){
	switch(backend){
		case BACKEND_GL4: return "OpenGL 4";
#ifdef _WIN32
		case BACKEND_DX11: return "DirectX 11";
#endif
#ifdef TOPL_ENABLE_VULKAN
		case BACKEND_VK: return "Vulkan";
#endif
		default: return "Unknown";
	}
}

const char* Topl_Runtime::getPipelineName(MODERN_Pipeline pipeline){
	static const char* names[MODERN_PIPELINE_COUNT] = { "Sky", "Lighting", "Toon", "Hologram", "Terrain", "Raymarch", "Spacetime", "Spectrum", "Picker" };
	return (pipeline < MODERN_PIPELINE_COUNT)? names[pipeline] : "Unknown";
}

void Topl_Runtime::setup(const char* execPath, const char* name){
#ifdef TOPL_ENABLE_VULKAN
	if(_backend == BACKEND_VK){ // modern shaders are compiled from source at runtime, Vulkan requires SPIR-V
		std::cout << "Modern pipelines are not available on Vulkan yet, using " << getBackendName(TOPL_DEFAULT_BACKEND) << std::endl;
		_backend = (TOPL_DEFAULT_BACKEND != BACKEND_VK)? TOPL_DEFAULT_BACKEND : BACKEND_GL4;
	}
#endif
	std::cout << name << " launching with " << getBackendName(_backend) << std::endl;

	_platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);

	_renderer = Topl_Factory::genRenderer(_backend, _platform);
	if(_renderer == nullptr){
		logMessage(MESSAGE_Exclaim, "Renderer creation failed, falling back to OpenGL");
		_backend = BACKEND_GL4;
		_renderer = Topl_Factory::genRenderer(_backend, _platform);
	}
	_renderer->setDrawMode(DRAW_Triangles);
	Platform::onMenuSelect = std::bind(&Topl_Runtime::menuSelect, this, std::placeholders::_1);

	genPipelines();
	setEnvironment(0);
	fps.reset(_startPos, _startYaw, _startPitch);
	fps.apply(&camera);

	buildScene(&_screenScene);
#ifdef TOPL_ENABLE_TEXTURES
	createOverlays();
#endif
}

void Topl_Runtime::genPipelines(){
	if(_backend == BACKEND_GL4){
		_skyVShader = Sky_VertexShader_GL4(); _skyPShader = Sky_PixelShader_GL4();
		_lightingVShader = Lighting_VertexShader_GL4(); _lightingPShader = Lighting_PixelShader_GL4();
		_toonVShader = Toon_VertexShader_GL4(); _toonPShader = Toon_PixelShader_GL4();
		_hologramVShader = Hologram_VertexShader_GL4(); _hologramPShader = Hologram_PixelShader_GL4();
		_terrainVShader = Terrain_VertexShader_GL4(); _terrainPShader = Terrain_PixelShader_GL4();
		_raymarchVShader = Raymarch_VertexShader_GL4(); _raymarchPShader = Raymarch_PixelShader_GL4();
		_spacetimeVShader = Spacetime_VertexShader_GL4(); _spacetimePShader = Spacetime_PixelShader_GL4();
		_spectrumVShader = Spectrum_VertexShader_GL4(); _spectrumPShader = Spectrum_PixelShader_GL4();
		_pickerVShader = Picker_VertexShader_GL4(); _pickerPShader = Picker_PixelShader_GL4();
#ifdef TOPL_ENABLE_TEXTURES
		_colorVShader = Color_VertexShader_GL4(); _colorPShader = Color_PixelShader_GL4();
		_texVShader = Textured_VertexShader_GL4(); _texPShader = Textured_PixelShader_GL4();
#endif
	}
#ifdef _WIN32
	else if(_backend == BACKEND_DX11){
		_skyVShader = Sky_VertexShader_DX11(); _skyPShader = Sky_PixelShader_DX11();
		_lightingVShader = Lighting_VertexShader_DX11(); _lightingPShader = Lighting_PixelShader_DX11();
		_toonVShader = Toon_VertexShader_DX11(); _toonPShader = Toon_PixelShader_DX11();
		_hologramVShader = Hologram_VertexShader_DX11(); _hologramPShader = Hologram_PixelShader_DX11();
		_terrainVShader = Terrain_VertexShader_DX11(); _terrainPShader = Terrain_PixelShader_DX11();
		_raymarchVShader = Raymarch_VertexShader_DX11(); _raymarchPShader = Raymarch_PixelShader_DX11();
		_spacetimeVShader = Spacetime_VertexShader_DX11(); _spacetimePShader = Spacetime_PixelShader_DX11();
		_spectrumVShader = Spectrum_VertexShader_DX11(); _spectrumPShader = Spectrum_PixelShader_DX11();
		_pickerVShader = Picker_VertexShader_DX11(); _pickerPShader = Picker_PixelShader_DX11();
#ifdef TOPL_ENABLE_TEXTURES
		_colorVShader = Color_VertexShader_DX11(); _colorPShader = Color_PixelShader_DX11();
		_texVShader = Textured_VertexShader_DX11(); _texPShader = Textured_PixelShader_DX11();
#endif
	}
#endif

	for(unsigned p = 0; p < MODERN_PIPELINE_COUNT; p++){
		_pipelines[p] = Topl_Factory::genPipeline(_backend, _vertexShaders[p], _pixelShaders[p]);
		if(_pipelines[p] == nullptr || !_pipelines[p]->isReady)
			std::cout << "Pipeline " << getPipelineName((MODERN_Pipeline)p) << " failed to build" << std::endl;
	}
#ifdef TOPL_ENABLE_TEXTURES
	_colorPipeline = Topl_Factory::genPipeline(_backend, &_colorVShader, &_colorPShader);
	_texPipeline = Topl_Factory::genPipeline(_backend, &_texVShader, &_texPShader);
#endif
	Topl_Factory::switchPipeline(_renderer, _pipelines[MODERN_Lighting]);
}

void Topl_Runtime::setEnvironment(unsigned index){
	Modern_SceneState& state = Modern_VertexShader::getState();
	_environment = index % RUNTIME_ENVIRONMENTS;

	switch(_environment){
		case 1: // cloudy sunset
			state.lightPos = Modern_Float4(0.85F, 0.2F, 0.25F, -1.1F);
			state.lightColor = Modern_Float4(1.0F, 0.62F, 0.4F, 0.45F);
			state.fog = Modern_Float4(0.86F, 0.58F, 0.46F, 0.016F);
			break;
		case 2: // starry night
			state.lightPos = Modern_Float4(-0.35F, 0.55F, 0.4F, -0.45F);
			state.lightColor = Modern_Float4(0.55F, 0.65F, 1.0F, 0.3F);
			state.fog = Modern_Float4(0.05F, 0.07F, 0.14F, 0.02F);
			break;
		case 3: // aurora
			state.lightPos = Modern_Float4(0.2F, 0.6F, -0.5F, -0.4F);
			state.lightColor = Modern_Float4(0.45F, 0.9F, 0.75F, 0.3F);
			state.fog = Modern_Float4(0.03F, 0.06F, 0.1F, 0.016F);
			break;
		default: // clear day
			state.lightPos = Modern_Float4(0.45F, 0.6F, 0.35F, -1.1F);
			state.lightColor = Modern_Float4(1.0F, 0.95F, 0.85F, 0.5F);
			state.fog = Modern_Float4(0.62F, 0.72F, 0.85F, 0.012F);
	}
}

void Topl_Runtime::buildScene(Topl_Scene* scene){
	if(scene == nullptr) return;
	Topl_Factory::switchPipeline(_renderer, _pipelines[MODERN_Lighting]);
	_renderer->buildScene(scene);
}

// Rendering

void Topl_Runtime::applyEdits(Geo_Actor* actor){
	auto entry = _edits.find(actor);
	if(entry == _edits.end()) return;
	const Runtime_Edit& edit = entry->second;

	float scale = edit.scale * ((edit.mesh >= 0)? meshLibrary.getNormScale(edit.mesh) : 1.0F);
	actor->setSize(*actor->getSize() * scale);
	actor->updatePos({ 0.0F, edit.lift, 0.0F });
	if(edit.isSpinning) actor->updateRot({ 0.0F, 0.0F, (float)elapsed * 1.5F });

	Modern_ActorProps& props = Modern_VertexShader::getActorProps()[actor];
	if(edit.color >= 0){
		float alpha = props.color[3];
		props.color = colorPalette[edit.color % RUNTIME_COLORS];
		props.color[3] = alpha;
	}
	if(edit.material >= 0){
		float smooth = props.params[3];
		props.params = materialParams[edit.material % RUNTIME_MATERIALS];
		props.params[3] = smooth;
	}
	if(edit.mesh >= 0) props.params[3] = (meshLibrary.isSmooth(edit.mesh))? 1.0F : 0.0F;
	if(edit.texture >= 0) props.tex = Modern_Float4((edit.texture == 0)? 0.0F : 0.85F, 1.0F, 0.0F, 0.0F);
}

void Topl_Runtime::renderScene(Topl_Scene* scene, MODERN_Pipeline pipeline, int mode, DRAW_Mode drawMode){
	if(scene == nullptr || pipeline >= MODERN_PIPELINE_COUNT || _pipelines[pipeline] == nullptr) return;

	struct Saved { Geo_Actor* actor; Vec3f pos, rot, size; Modern_ActorProps props; };
	std::vector<Saved> saved; // edits only last for this draw so demos keep their own values
	for(unsigned a = 0; a < scene->getActorCount(); a++){
		Geo_Actor* actor = const_cast<Geo_Actor*>(scene->getGeoActor(a));
		if(actor == nullptr || _edits.find(actor) == _edits.end()) continue;
		saved.push_back({ actor, *actor->getPos(), *actor->getRot(), *actor->getSize(), Modern_VertexShader::getActorProps()[actor] });
		applyEdits(actor);
	}

	Topl_Factory::switchPipeline(_renderer, _pipelines[pipeline]);
	_vertexShaders[pipeline]->setMode(mode);
	_renderer->setDrawMode(drawMode);
	_renderer->updateScene(scene);
	_renderer->drawScene(scene);

	for(auto s = saved.begin(); s != saved.end(); s++){
		s->actor->setPos(s->pos);
		s->actor->setRot(s->rot);
		s->actor->setSize(s->size);
		Modern_VertexShader::getActorProps()[s->actor] = s->props;
	}
}

void Topl_Runtime::renderSky(){ renderScene(&_screenScene, MODERN_Sky, (int)_environment); }

void Topl_Runtime::renderFullscreen(MODERN_Pipeline pipeline, int mode){ renderScene(&_screenScene, pipeline, mode); }

// Editing

void Topl_Runtime::addEditable(Geo_Actor* actor, Topl_Scene* scene){
	if(actor == nullptr || scene == nullptr) return;
	_editables.push_back(std::make_pair(actor, scene));
	_pickScene.addGeometry(actor);
}

Topl_Scene* Topl_Runtime::getEditableScene(const Geo_Actor* actor){
	for(auto e = _editables.begin(); e != _editables.end(); e++) if(e->first == actor) return e->second;
	return nullptr;
}

void Topl_Runtime::select(Geo_Actor* actor){
	_selected = actor;
	Modern_VertexShader::getHighlight() = actor;
	if(actor != nullptr) std::cout << "Selected " << actor->getName() << ", menu changes apply to it" << std::endl;
	else std::cout << "Nothing selected, menu changes apply to every object" << std::endl;
}

std::vector<Runtime_Edit*> Topl_Runtime::getTargetEdits(std::vector<Geo_Actor*>* actors){
	std::vector<Runtime_Edit*> targetEdits;
	for(auto e = _editables.begin(); e != _editables.end(); e++)
		if(_selected == nullptr || e->first == _selected){
			targetEdits.push_back(&_edits[e->first]);
			if(actors != nullptr) actors->push_back(e->first);
		}
	return targetEdits;
}

float Topl_Runtime::getMeshHeight(const Geo_Mesh* mesh){
	if(mesh == nullptr) return 1.0F;
	auto entry = _meshHeights.find(mesh);
	if(entry != _meshHeights.end()) return entry->second;

	float yMin = 0.0F, yMax = 0.0F;
	for(unsigned v = 0; v < mesh->getVertexCount(); v++){
		float y = mesh->getVertices()[v].position.data[1];
		if(y < yMin) yMin = y;
		if(y > yMax) yMax = y;
	}
	float height = (yMax - yMin > 0.0001F)? yMax - yMin : 1.0F;
	_meshHeights[mesh] = height;
	return height;
}

void Topl_Runtime::selectMesh(unsigned index){
	index %= RUNTIME_MESHES;
	std::vector<Geo_Actor*> actors;
	std::vector<Runtime_Edit*> targets = getTargetEdits(&actors);
	std::vector<Topl_Scene*> rebuilds;

	for(unsigned t = 0; t < targets.size(); t++){
		targets[t]->mesh = (int)index;
		actors[t]->setMesh(meshLibrary.getMesh(index));
		Topl_Scene* scene = getEditableScene(actors[t]);
		if(std::find(rebuilds.begin(), rebuilds.end(), scene) == rebuilds.end()) rebuilds.push_back(scene);
		onMeshChanged(actors[t]);
	}
	for(auto s = rebuilds.begin(); s != rebuilds.end(); s++){ // new vertex buffers for the swapped meshes
		buildScene(*s);
		_renderer->texturizeScene(*s);
	}
	std::cout << "Mesh: " << Runtime_MeshLibrary::getName(index) << std::endl;
}

void Topl_Runtime::selectTexture(unsigned index){
	index %= RUNTIME_TEXTURES;
	std::vector<Geo_Actor*> actors;
	std::vector<Runtime_Edit*> targets = getTargetEdits(&actors);
#ifdef TOPL_ENABLE_TEXTURES
	if(index > 0) genTexture(index);
	std::vector<Topl_Scene*> refreshes;
	for(unsigned t = 0; t < targets.size(); t++){
		targets[t]->texture = (int)index;
		if(index == 0) continue;
		Topl_Scene* scene = getEditableScene(actors[t]);
		scene->addTexture(actors[t]->getName(), &_textures[index]);
		if(std::find(refreshes.begin(), refreshes.end(), scene) == refreshes.end()) refreshes.push_back(scene);
	}
	for(auto s = refreshes.begin(); s != refreshes.end(); s++) _renderer->texturizeScene(*s);
	std::cout << "Texture: " << textureItems[index] << std::endl;
#else
	std::cout << "Textures require Rasteron (SUPPORT_TEXTURES)" << std::endl;
#endif
}

void Topl_Runtime::onCameraItem(unsigned index){
	switch(index % RUNTIME_PANE_ITEMS){
		case 0: fov = (fov > 0.4F)? fov - 0.17F : fov; break;
		case 1: fov = (fov < 1.9F)? fov + 0.17F : fov; break;
		case 2: fps.zoom = (fps.zoom * 1.25F < 6.0F)? fps.zoom * 1.25F : 6.0F; break;
		case 3: fps.zoom = (fps.zoom / 1.25F > 0.25F)? fps.zoom / 1.25F : 0.25F; break;
		case 4: fps.isFlying = !fps.isFlying; break;
		case 5: fps.moveSpeed = (fps.moveSpeed / 1.5F > 0.5F)? fps.moveSpeed / 1.5F : 0.5F; break;
		case 6: fps.moveSpeed = (fps.moveSpeed * 1.5F < 60.0F)? fps.moveSpeed * 1.5F : 60.0F; break;
		case 7: setEnvironment(_environment + 1); break;
		case 8: fps.reset(_startPos, _startYaw, _startPitch); fov = MODERN_FOV; break;
	}
	std::cout << "Camera: " << cameraItems[index % RUNTIME_PANE_ITEMS]
		<< " (fov " << (int)(fov * 57.2958F) << ", zoom " << fps.zoom << ", speed " << fps.moveSpeed << ((fps.isFlying)? ", flying)" : ", walking)") << std::endl;
}

void Topl_Runtime::onObjectItem(unsigned index){
	index %= RUNTIME_PANE_ITEMS;
	if(index == 8){ select(nullptr); return; }

	std::vector<Geo_Actor*> actors;
	std::vector<Runtime_Edit*> targets = getTargetEdits(&actors);
	for(unsigned t = 0; t < targets.size(); t++){
		Runtime_Edit* edit = targets[t];
		switch(index){
			case 0: edit->scale /= 1.2F; break;
			case 1: edit->scale *= 1.2F; break;
			case 2: edit->lift += 0.5F; break;
			case 3: edit->lift -= 0.5F; break;
			case 4: edit->isSpinning = !edit->isSpinning; break;
			case 5: edit->color = (edit->color + 1) % RUNTIME_COLORS; break;
			case 6: if(isMaterialEditable()) edit->material = (edit->material + 1) % RUNTIME_MATERIALS; break;
			case 7: { // mesh and texture stay, everything else returns to the demo's values
				int mesh = edit->mesh, texture = edit->texture;
				*edit = Runtime_Edit();
				edit->mesh = mesh; edit->texture = texture;
				break;
			}
		}
	}
	std::cout << "Object: " << objectItems[index];
	if(index == 6 && !isMaterialEditable()) std::cout << " (not used by this demo)";
	else if(index == 6 && !targets.empty()) std::cout << " (" << materialNames[targets.front()->material % RUNTIME_MATERIALS] << ")";
	std::cout << " on " << ((_selected != nullptr)? _selected->getName() : std::string("every object")) << std::endl;
}

void Topl_Runtime::onMediaItem(unsigned index){
	switch(index % RUNTIME_PANE_ITEMS){
		case 0: elapsed = 0.0; break;
		case 1: isPaused = !isPaused; audio.setPaused(isPaused); break;
		case 2: timeScale = (timeScale > 0.125)? timeScale * 0.5 : timeScale; break;
		case 3: timeScale = (timeScale < 8.0)? timeScale * 2.0 : timeScale; break;
		case 4: sequence.record(getTimelineTime(), fps); break;
		case 5: sequence.isPlaying = !sequence.isPlaying && sequence.getCount() > 0; break;
		case 6: sequence.clear(); break;
		case 7: audio.play((audio.isPlaying())? audio.getTrack() + 1 : audio.getTrack()); break;
		case 8: audio.stop(); break;
	}
	std::cout << "Media: " << mediaItems[index % RUNTIME_PANE_ITEMS] << " (time " << (int)getTimelineTime() << "s, speed " << timeScale
		<< "x, " << sequence.getCount() << " camera keys" << ((sequence.isPlaying)? " playing" : "") << ((isPaused)? ", paused)" : ")") << std::endl;
}

void Topl_Runtime::menuSelect(unsigned short menuID){
	if(menuID >= CAMERA1 && menuID <= CAMERA4) setEnvironment(menuID - CAMERA1);
	else if(menuID >= FOV1 && menuID <= FOV6){
		static const unsigned fovItems[6] = { 1, 0, 2, 3, 6, 5 }; // wider, narrower, zoom in, zoom out, faster, slower
		onCameraItem(fovItems[menuID - FOV1]);
	}
	else if(menuID >= PIPELINE1 && menuID <= PIPELINE9) onPipelineSelect(menuID - PIPELINE1);
	else if(menuID >= SHAPES1 && menuID <= SHAPES9) selectMesh(menuID - SHAPES1);
	else if(menuID >= SAMPLERS1 && menuID <= SAMPLERS9) selectTexture(menuID - SAMPLERS1);
	else if(menuID >= SOUNDS1 && menuID <= SOUNDS9) audio.play(menuID - SOUNDS1);
}

double Topl_Runtime::getTimelineTime() const {
	double t = fmod(elapsed, RUNTIME_TIMELINE_LENGTH);
	return (t < 0.0)? t + RUNTIME_TIMELINE_LENGTH : t;
}

// Picking

void Topl_Runtime::pickWorld(){
#ifdef TOPL_ENABLE_TEXTURES
	if(_pickScene.getActorCount() == 0 || !isPickEnabled()) return;

	renderScene(&_pickScene, MODERN_Picker, 0);
	unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	_renderer->clear();

	unsigned c1 = color & 0xFF, c2 = (color >> 8) & 0xFF, c3 = (color >> 16) & 0xFF;
	if(c1 != c2 || c2 != c3) return; // only gray pixels come from the picker, anything else is background
	for(auto e = _editables.begin(); e != _editables.end(); e++)
		if(e->first->isShown && Picker_VertexShader::getPickByte(e->first) == c1){
			select((_selected == e->first)? nullptr : e->first); // clicking the selection again deselects it
			return;
		}
#endif
}

#ifdef TOPL_ENABLE_TEXTURES
void Topl_Runtime::createOverlays(){
	Runtime_Overlays& o = _overlays;
	o.cameraRow.scale({ 1.5F * 0.66F, 0.15F * 0.75F, 1.0F });
	o.cameraRow.shift({ 0.0F, 0.97F, 0.0F });
	o.objectRow.scale({ 1.5F * 0.66F, 0.15F * 0.75F, 1.0F });
	o.objectRow.shift({ 0.0F, 0.9F, 0.0F });
	o.meshList.scale({ 0.12F * 0.75F, 1.15F * 0.75F, 1.0F });
	o.meshList.shift({ -0.975F, 0.0F, 0.0F });
	o.textureList.scale({ 0.12F * 0.75F, 1.15F * 0.75F, 1.0F });
	o.textureList.shift({ 0.975F, 0.0F, 0.0F });
	o.mediaRow.scale({ 1.5F * 0.66F, 0.15F * 0.75F, 1.0F });
	o.mediaRow.shift({ 0.0F, -0.9F, 0.0F });
	o.timeline.scale({ 1.5F * 0.66F, 0.15F * 0.75F, 1.0F });
	o.timeline.shift({ 0.0F, -0.97F, 0.0F });

	Topl_Factory::switchPipeline(_renderer, _texPipeline);
	for(unsigned p = 0; p < PANE_COUNT; p++){
		Geo_Billboard* board = o.boards[p];
		board->getGeoActor(board->getActorCount() - 1)->updateSize({ 0.01F, 0.01F, 0.0F }); // border around the panes
		_renderer->buildScene(&o.scenes[p]);
		_renderer->texturizeScene(&o.scenes[p]);
	}
	Topl_Factory::switchPipeline(_renderer, _pipelines[MODERN_Lighting]);
}

void Topl_Runtime::renderOverlays(){
	if(_texPipeline == nullptr) return;

	unsigned step = (unsigned)(getTimelineTime() / RUNTIME_TIMELINE_LENGTH * RUNTIME_TIMELINE_STEPS);
	if((int)step != _overlays.sliderStep){ // the slider image only changes once per step
		_overlays.sliderStep = (int)step;
		_overlays.timeline.setState(0, (double)step / (RUNTIME_TIMELINE_STEPS - 1), 0.0);
		_renderer->texturizeScene(&_overlays.scenes[PANE_Timeline]);
	}

	Topl_Factory::switchPipeline(_renderer, _texPipeline);
	_texVShader.setMode(TEX_BASE);
	_renderer->setDrawMode(DRAW_Triangles);
	for(unsigned p = 0; p < PANE_COUNT; p++){
		_renderer->updateScene(&_overlays.scenes[p]);
		_renderer->drawScene(&_overlays.scenes[p]);
	}
}

bool Topl_Runtime::pickOverlays(){
	if(_colorPipeline == nullptr) return false;

	Topl_Factory::switchPipeline(_renderer, _colorPipeline);
	_colorVShader.setMode(COLORED_ID);
	_renderer->setDrawMode(DRAW_Triangles);
	for(unsigned p = 0; p < PANE_COUNT; p++){
		_renderer->updateScene(&_overlays.scenes[p]);
		_renderer->drawScene(&_overlays.scenes[p]);
	}
	unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	_renderer->clear();

	for(unsigned p = 0; p < PANE_COUNT; p++){
		Geo_Actor* actor = _overlays.scenes[p].getPickActor(color);
		if(actor == nullptr || actor->getName().find(_overlays.boards[p]->getPrefix()) == std::string::npos) continue;

		Geo_Billboard* board = _overlays.boards[p];
		for(unsigned short c = 0; c < board->getActorCount() - 1; c++)
			if(board->getGeoActor(c) == actor){
				pressPane((RUNTIME_Pane)p, c);
				return true;
			}
		return true; // the border of a pane still blocks clicks into the world
	}
	return false;
}

void Topl_Runtime::scrubTimeline(){ // reads the cursor position inside the slider
	Topl_Factory::switchPipeline(_renderer, _colorPipeline);
	_colorVShader.setMode(COLORED_COORD);
	_renderer->setDrawMode(DRAW_Triangles);
	_renderer->updateScene(&_overlays.scenes[PANE_Timeline]);
	_renderer->drawScene(&_overlays.scenes[PANE_Timeline]);
	unsigned color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	_renderer->clear();

	double x = ((color & 0xFF0000) >> 16) / 255.0;
	elapsed = x * RUNTIME_TIMELINE_LENGTH;
}

void Topl_Runtime::pressPane(RUNTIME_Pane pane, unsigned short cell){
	if(pane == PANE_Timeline){
		_isTimelineDrag = true;
		scrubTimeline();
		return;
	}

	unsigned index = (_overlays.isReversed(pane))? RUNTIME_PANE_ITEMS - 1 - cell : cell;
	_overlays.boards[pane]->setState(cell, true); // highlights the pressed button
	_renderer->texturizeScene(&_overlays.scenes[pane]);

	switch(pane){
		case PANE_Camera: onCameraItem(index); break;
		case PANE_Object: onObjectItem(index); break;
		case PANE_Meshes: selectMesh(index); break;
		case PANE_Textures: selectTexture(index); break;
		case PANE_Media: onMediaItem(index); break;
		default: std::cout << paneNames[pane] << " pane item " << index + 1 << std::endl;
	}
}

void Topl_Runtime::genTexture(unsigned index){
	if(index == 0 || index >= RUNTIME_TEXTURES) return;
	if(_isTextureMade[index] && index != RUNTIME_TEXTURES - 1) return; // the random image is chosen again every time

	ImageSize size = { SAMPLER_WIDTH, SAMPLER_HEIGHT };
	unsigned color1 = (RAND_COLOR() & 0xFFFFFF) + 0xFF000000;
	unsigned color2 = (RAND_COLOR() & 0xFFFFFF) + 0xFF000000;
	switch(index){
		case 1: _textures[index] = Topl_Sampler_2D(checkeredImgOp(size, { 8, 8, color1, color2 })); break;
		case 2: _textures[index] = Topl_Sampler_2D(linedImgOp(size, color1, color2, 16, 0.0)); break;
		case 3: _textures[index] = Topl_Sampler_Gradient(SIDE_Radial, color1, color2); break;
		case 4: _textures[index] = Topl_Sampler_Noise({ 16, 16, color1, color2 }); break;
		case 5: _textures[index] = Topl_Sampler_Noise({ 8, 8, color1, color2 }, (unsigned)4); break;
		case 6: _textures[index] = Topl_Sampler_2D(noiseImgOp_tiled(size, { 16, 16, color1, color2 })); break;
		case 7: _textures[index] = Topl_Sampler_File(_pickScene.texImgPaths[0]); break;
		default: _textures[index] = Topl_Sampler_File(_pickScene.texImgPaths[rand() % MAX_TEX_BINDINGS]);
	}
	_isTextureMade[index] = true;
}
#endif

// Frame

void Topl_Runtime::handleKeys(){
	if(input.isPressed(KEY_ModeNext) || input.isPressed(KEY_ModePrev)){
		shaderMode = (shaderMode + ((input.isPressed(KEY_ModeNext))? 1 : RUNTIME_MODES - 1)) % RUNTIME_MODES;
		std::cout << "Shader mode " << shaderMode << std::endl;
	}
	if(input.isPressed(KEY_Reset)) onCameraItem(8);
	if(input.isPressed(KEY_Fly)) onCameraItem(4);
	if(input.isPressed(KEY_Pause)) onMediaItem(1);
	if(input.isPressed(KEY_Environment)) setEnvironment(_environment + 1);
	if(input.isPressed(KEY_Overlay)) isOverlays = !isOverlays;
	if(input.isPressed(KEY_Help)) printControls();

	for(unsigned k = 0; k < KEY_COUNT; k++)
		if(input.isPressed((RUNTIME_Key)k)) onPress((RUNTIME_Key)k);
}

void Topl_Runtime::updateFrame(double frameSecs){
	input.poll(_platform->getContext());
	handleKeys();

	// Clicks go to the menu panes first and then to the editable objects, dragging elsewhere looks around
	if(input.isMousePressed()){
#ifdef TOPL_ENABLE_TEXTURES
		_isPaneDrag = isOverlays && pickOverlays();
#endif
		if(!_isPaneDrag) pickWorld();
	}
#ifdef TOPL_ENABLE_TEXTURES
	else if(_isTimelineDrag && input.isMouseHeld()) scrubTimeline();
#endif
	if(!input.isMouseHeld()){ _isPaneDrag = false; _isTimelineDrag = false; }

	if(!isPaused) elapsed += frameSecs * timeScale;
	frames++;

	if(sequence.isPlaying) sequence.sample(getTimelineTime(), &fps); // recorded camera path drives the view
	else fps.update(input, frameSecs, !_isPaneDrag);
	fps.apply(&camera);

	Modern_SceneState& state = Modern_VertexShader::getState();
	state.time = Modern_Float4((float)elapsed, (float)frameSecs, (float)frames, 0.0F);
	state.lens[0] = fov;
	state.lens[1] = input.getAspect();
}

void Topl_Runtime::printControls() const {
	std::cout << std::endl
		<< "Controls" << std::endl
		<< "  W A S D   move          Mouse drag / Arrows   look" << std::endl
		<< "  Q / E     down / up     Space                 jump or rise" << std::endl
		<< "  Shift     sprint        F                     fly or walk" << std::endl
		<< "  Z / X     zoom          R                     reset camera" << std::endl
		<< "  M / N     shader mode   T                     environment" << std::endl
		<< "  P         pause time    O                     show or hide menus" << std::endl
		<< "  Click an object to select it, click it again to deselect, H prints this help" << std::endl
		<< std::endl << "Menu panes (buttons 1 to 9)" << std::endl;
	std::cout << "  Top      "; for(unsigned i = 0; i < RUNTIME_PANE_ITEMS; i++) std::cout << i + 1 << " " << cameraItems[i] << ((i < RUNTIME_PANE_ITEMS - 1)? ", " : "\n");
	std::cout << "  Second   "; for(unsigned i = 0; i < RUNTIME_PANE_ITEMS; i++) std::cout << i + 1 << " " << objectItems[i] << ((i < RUNTIME_PANE_ITEMS - 1)? ", " : "\n");
	std::cout << "  Left     "; for(unsigned i = 0; i < RUNTIME_MESHES; i++) std::cout << i + 1 << " " << Runtime_MeshLibrary::getName(i) << ((i < RUNTIME_MESHES - 1)? ", " : "\n");
	std::cout << "  Right    "; for(unsigned i = 0; i < RUNTIME_TEXTURES; i++) std::cout << i + 1 << " " << textureItems[i] << ((i < RUNTIME_TEXTURES - 1)? ", " : "\n");
	std::cout << "  Bottom   "; for(unsigned i = 0; i < RUNTIME_PANE_ITEMS; i++) std::cout << i + 1 << " " << mediaItems[i] << ((i < RUNTIME_PANE_ITEMS - 1)? ", " : "\n");
	std::cout << "  Slider   drag to scrub the " << (int)RUNTIME_TIMELINE_LENGTH << " second timeline" << std::endl;
}

void Topl_Runtime::run(){
	init();
	printControls();
	_ticker.reset();

	while(_platform->handleEvents()){
		double frameSecs = _ticker.getRelSecs();
		if(frameSecs > RUNTIME_FRAME_MAX) frameSecs = RUNTIME_FRAME_MAX;
		else if(frameSecs < 0.0) frameSecs = 0.0;

		updateFrame(frameSecs);
		_renderer->clear();
		loop(frameSecs);
#ifdef TOPL_ENABLE_TEXTURES
		if(isOverlays) renderOverlays();
#endif
		_renderer->present();
	}
}
