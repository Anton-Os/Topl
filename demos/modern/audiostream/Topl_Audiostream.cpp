#include "Topl_Audiostream.hpp"

static const char* sourceNames[3] = { "Synthesized stream", "Microphone", "System audio (loopback)" };
static const unsigned ringMeshes[STREAM_RINGS] = { 8, 6, 5, 1, 3 }; // gem, cone, triangle prism, cube, hex prism
static const float ringRadii[STREAM_RINGS] = { 2.0F, 3.4F, 4.8F, 6.4F, 8.2F };
static const float ringSizes[STREAM_RINGS] = { 0.42F, 0.38F, 0.4F, 0.36F, 0.34F };
static const unsigned foldOptions[3] = { 4, 6, 12 };

static float clamp01(float x){ return (x < 0.0F)? 0.0F : (x > 1.0F)? 1.0F : x; }

static float goertzel(const float* samples, unsigned count, float freq, float sampleRate){
	float coeff = 2.0F * (float)cos(2.0 * MATH_PI * freq / sampleRate);
	float s1 = 0.0F, s2 = 0.0F;
	for(unsigned n = 0; n < count; n++){
		float window = 0.5F - 0.5F * (float)cos(2.0 * MATH_PI * n / (count - 1)); // hann window
		float s = samples[n] * window + coeff * s1 - s2;
		s2 = s1; s1 = s;
	}
	float power = s1 * s1 + s2 * s2 - coeff * s1 * s2;
	return (power > 0.0F)? (float)sqrt(power) / (count * 0.25F) : 0.0F;
}

// Capture

#ifdef TOPL_ENABLE_AUDIO
void Stream_Capture::onData(ma_device* device, void* output, const void* input, ma_uint32 frameCount){ // runs on the device thread
	Stream_Capture* capture = (Stream_Capture*)device->pUserData;
	const float* samples = (const float*)input;
	if(capture == nullptr || samples == nullptr) return;

	unsigned long long written = capture->_written.load(std::memory_order_relaxed);
	for(ma_uint32 s = 0; s < frameCount; s++) capture->_ring[(size_t)((written + s) & (STREAM_RING_BUFFER - 1))] = samples[s];
	capture->_written.store(written + frameCount, std::memory_order_release);
	(void)output;
}
#endif

unsigned Stream_Capture::getDeviceCount(){
#ifdef TOPL_ENABLE_AUDIO
	if(!_isContext) _isContext = ma_context_init(NULL, 0, NULL, &_context) == MA_SUCCESS;
	if(!_isContext) return 0;
	ma_device_info* playbackInfos = nullptr;
	ma_uint32 playbackCount = 0;
	if(ma_context_get_devices(&_context, &playbackInfos, &playbackCount, &_captureInfos, &_captureCount) != MA_SUCCESS) _captureCount = 0;
	return _captureCount;
#else
	return 0;
#endif
}

std::string Stream_Capture::getDeviceName(unsigned deviceIndex){
#ifdef TOPL_ENABLE_AUDIO
	if(getDeviceCount() == 0) return "default device";
	return std::string(_captureInfos[deviceIndex % _captureCount].name);
#else
	return "no audio support";
#endif
}

bool Stream_Capture::start(STREAM_Source source, unsigned deviceIndex){
#ifdef TOPL_ENABLE_AUDIO
	stop();
	if(source == STREAM_Synth) return false;
	unsigned count = getDeviceCount();
	if(!_isContext) return false;

	ma_device_config config = ma_device_config_init((source == STREAM_Loopback)? ma_device_type_loopback : ma_device_type_capture);
	config.capture.format = ma_format_f32;
	config.capture.channels = 1; // miniaudio mixes the input down to mono
	config.sampleRate = 0; // native rate of the device
	config.dataCallback = onData;
	config.pUserData = this;
	if(source == STREAM_Microphone && count > 0) config.capture.pDeviceID = &_captureInfos[deviceIndex % count].id;

	if(ma_device_init(&_context, &config, &_device) != MA_SUCCESS) return false;
	_isDevice = true;
	_sampleRate = (_device.sampleRate > 0)? (float)_device.sampleRate : 48000.0F;
	_written.store(0);
	if(ma_device_start(&_device) != MA_SUCCESS){ stop(); return false; }
	return true;
#else
	return false;
#endif
}

void Stream_Capture::stop(){
#ifdef TOPL_ENABLE_AUDIO
	if(_isDevice) ma_device_uninit(&_device);
#endif
	_isDevice = false;
}

void Stream_Capture::close(){
#ifdef TOPL_ENABLE_AUDIO
	if(_isContext) ma_context_uninit(&_context);
#endif
	_isContext = false;
}

bool Stream_Capture::read(float* samples, unsigned count) const {
	unsigned long long written = _written.load(std::memory_order_acquire);
	if(!_isDevice || written == 0) return false;
	for(unsigned s = 0; s < count; s++){
		unsigned long long back = count - s; // samples before the newest one
		samples[s] = (back <= written)? _ring[(size_t)((written - back) & (STREAM_RING_BUFFER - 1))] : 0.0F;
	}
	return true;
}

// Analysis

void Topl_Audiostream::synthesize(float* levels){
	float t = (float)elapsed;
	float beat = (float)pow(fabs(sin(t * MATH_PI * 2.0F * 0.9F)), 10.0F); // 108 bpm pulse
	for(unsigned b = 0; b < STREAM_BANDS; b++){
		float f = (float)b / STREAM_BANDS;
		float melody = 0.5F + 0.5F * (float)sin(t * (0.9F + f * 2.5F) + b * 1.1F);
		float shimmer = 0.5F + 0.5F * (float)sin(t * 6.0F + b * 2.7F);
		levels[b] = clamp01(beat * (1.0F - f) * 0.95F + melody * 0.4F * (0.4F + f) + shimmer * 0.2F * f);
	}
}

void Topl_Audiostream::analyze(double frameSecs){
	float dt = (float)frameSecs;
	float raw[STREAM_BANDS];
	bool isLive = _capture.isActive() && _capture.read(_window.data(), STREAM_WINDOW);

	if(isLive){
		double sum = 0.0;
		for(unsigned s = 0; s < STREAM_WINDOW; s++) sum += _window[s] * _window[s];
		float rms = (float)sqrt(sum / STREAM_WINDOW);
		_loudness = clamp01((20.0F * (float)log10(rms + 1e-6F) + 60.0F) / 50.0F);

		for(unsigned b = 0; b < STREAM_BANDS; b++){
			float freq = STREAM_MIN_FREQ * (float)pow(STREAM_MAX_FREQ / STREAM_MIN_FREQ, (float)b / (STREAM_BANDS - 1)); // log spaced bands
			float amp = goertzel(_window.data(), STREAM_WINDOW, freq, _capture.getSampleRate());
			float level = clamp01((20.0F * (float)log10(amp + 1e-7F) + 70.0F) / 60.0F); // -70 dB to -10 dB
			_floors[b] = (level < _floors[b])? level : _floors[b] + dt * 0.04F; // floor drops at once and rises slowly
			raw[b] = clamp01((level - _floors[b]) * 2.5F); // rooms and microphones differ, so levels sit above the noise floor
		}
	} else {
		synthesize(raw);
		_loudness = 0.0F;
		for(unsigned b = 0; b < STREAM_BANDS; b++) _loudness += raw[b] / STREAM_BANDS;
	}

	float decay = (float)exp(-frameSecs * 6.0); // fast attack, smooth release
	for(unsigned b = 0; b < STREAM_BANDS; b++) _levels[b] = (raw[b] > _levels[b] * decay)? raw[b] : _levels[b] * decay;

	float bass = (_levels[0] + _levels[1] + _levels[2] + _levels[3]) * 0.25F;
	_bassAverage += (bass - _bassAverage) * (1.0F - (float)exp(-frameSecs * 2.0));
	_beatCooldown -= dt;
	if(bass > _bassAverage * 1.3F + 0.08F && _beatCooldown <= 0.0F){ _beat = 1.0F; _beatCooldown = 0.22F; }
	_beat *= (float)exp(-frameSecs * 5.0);
}

// Mandala

float Topl_Audiostream::getPetalBand(unsigned ring, unsigned petal) const {
	unsigned sector = _ringCounts[ring] / _folds; // petals between two mirror lines
	unsigned j = petal % sector;
	unsigned mirrored = (j < (sector + 1) / 2)? j : sector - 1 - j;
	float spread = (sector > 2)? (float)mirrored / ((sector - 1) * 0.5F) : 0.0F;
	float bandsPerRing = (float)STREAM_BANDS / STREAM_RINGS;
	float band = (ring + ((spread < 1.0F)? spread : 1.0F)) * bandsPerRing; // bass in the center, treble on the rim
	return (band < STREAM_BANDS - 1)? band : (float)(STREAM_BANDS - 1);
}

void Topl_Audiostream::arrange(double frameSecs){
	float t = (float)elapsed;
	float dt = (isPaused)? 0.0F : (float)(frameSecs * timeScale);

	for(unsigned r = 0; r < STREAM_RINGS; r++){
		float energy = 0.0F;
		for(unsigned b = 0; b < 3; b++) energy += _levels[(r * 3 + b) % STREAM_BANDS] / 3.0F;
		float direction = (r % 2 == 0)? 1.0F : -1.0F;
		_ringAngles[r] += dt * direction * (0.05F + energy * 0.6F + _beat * 0.9F) * _spinScale;

		unsigned count = _ringCounts[r];
		unsigned mesh = (ringMeshes[r] + _meshOffset) % RUNTIME_MESHES;
		float norm = meshLibrary.getNormScale(mesh);
		for(unsigned p = 0; p < count; p++){
			Geo_Actor* petal = &_petals[_ringStarts[r] + p];
			float band = getPetalBand(r, p);
			float level = _levels[(unsigned)(band + 0.5F) % STREAM_BANDS];
			float angle = (MATH_PI * 2.0F / count) * p + _ringAngles[r];
			float radius = ringRadii[r] * (1.0F + level * 0.12F + _beat * 0.06F * (r + 1));
			float depth = (_isLayered)? r * 2.6F + (float)sin(t * 0.7F + r) * 0.4F : r * 0.15F;

			float size = ringSizes[r] * norm;
			petal->setPos({ (float)cos(angle) * radius, (float)sin(angle) * radius, depth });
			petal->setRot({ angle, level * 0.8F, t * 0.6F * ((p % 2 == 0)? 1.0F : -1.0F) }); // points outward and twists
			petal->setSize({ size * (1.0F + level * 1.6F + _beat * 0.4F), size * (0.7F + level * 0.5F), size * (0.7F + level * 0.5F) });
		}
	}

	float bass = (_levels[0] + _levels[1] + _levels[2] + _levels[3]) * 0.25F;
	float coreSize = 1.2F + bass * 0.7F + _beat * 0.3F;
	_coreActor.setSize({ coreSize, coreSize, coreSize });
	_coreActor.setRot({ t * 0.2F, t * 0.15F, t * 0.3F });
	for(unsigned h = 0; h < 2; h++){
		float radius = ((h == 0)? ringRadii[0] * 0.62F : ringRadii[STREAM_RINGS - 1] * 1.12F) * (1.0F + _beat * 0.05F);
		_halos[h].setSize({ radius, radius, radius * 1.4F });
		_halos[h].setPos({ 0.0F, 0.0F, (_isLayered)? h * STREAM_RINGS * 2.6F : 0.0F });
		_halos[h].setRot({ t * ((h == 0)? 0.4F : -0.25F), (float)sin(t * 0.3F) * 0.15F, 0.0F });
	}
}

void Topl_Audiostream::setSource(STREAM_Source source){
	_source = source;
	bool isLive = (source != STREAM_Synth) && _capture.start(source, _deviceIndex);
	if(source == STREAM_Synth) _capture.stop();
	for(unsigned b = 0; b < STREAM_BANDS; b++) _floors[b] = 1.0F;

	if(isLive){
		std::cout << "Streaming from " << sourceNames[source];
		if(source == STREAM_Microphone) std::cout << ": " << _capture.getDeviceName(_deviceIndex);
		std::cout << " at " << (int)_capture.getSampleRate() << " Hz" << std::endl;
	} else {
		if(source != STREAM_Synth){
			std::cout << sourceNames[source] << " is unavailable";
			if(!_capture.isActive() && source == STREAM_Loopback) std::cout << " (loopback needs WASAPI on Windows)";
#ifndef TOPL_ENABLE_AUDIO
			std::cout << " (enable SUPPORT_AUDIO)";
#endif
			std::cout << ", using the synthesized stream" << std::endl;
		} else std::cout << "Synthesized stream" << std::endl;
		_source = STREAM_Synth;
	}
}

// Demo

void Topl_Audiostream::init(){
	setEnvironment(2); // starry night
	fps.isFlying = true;
	setStart(Vec3f({ 0.0F, 0.0F, -22.0F }), 0.0F, 0.0F);

	unsigned start = 0;
	for(unsigned r = 0; r < STREAM_RINGS; r++){
		_ringStarts[r] = start;
		for(unsigned p = 0; p < _ringCounts[r]; p++){
			Geo_Actor* petal = &_petals[start + p];
			petal->setName("stream_petal_" + std::to_string(r) + "_" + std::to_string(p));
			petal->setMesh(meshLibrary.getMesh(ringMeshes[r]));
			_mandalaScene.addGeometry(petal);
			Modern_VertexShader::setActorParams(petal, Modern_Float4(getPetalBand(r, p), 0.0F, 0.0F, 0.0F)); // band index, bar kind
		}
		start += _ringCounts[r];
	}
	Modern_VertexShader::setActorParams(&_coreActor, Modern_Float4(0.0F, 2.0F, 0.0F, 0.0F)); // pulsing orb kind
	Modern_VertexShader::setActorParams(&_halos[0], Modern_Float4(1.0F, 0.0F, 0.0F, 0.0F));
	Modern_VertexShader::setActorParams(&_halos[1], Modern_Float4((float)(STREAM_BANDS - 1), 0.0F, 0.0F, 0.0F));
	arrange(0.0);

	buildScene(&_mandalaScene);
	buildScene(&_coreScene);
	for(unsigned p = 0; p < STREAM_PETALS; p++) addEditable(&_petals[p], &_mandalaScene);
	addEditable(&_coreActor, &_coreScene);

	setSource(STREAM_Microphone); // falls back to the synthesized stream without a capture device
}

void Topl_Audiostream::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: setSource(STREAM_Microphone); break;
		case KEY_2: {
			unsigned count = _capture.getDeviceCount();
			_deviceIndex = (count > 0)? (_deviceIndex + 1) % count : 0;
			setSource(STREAM_Microphone);
			break;
		}
		case KEY_3: setSource(STREAM_Loopback); break;
		case KEY_4: setSource(STREAM_Synth); break;
		case KEY_5: {
			unsigned option = 0;
			for(unsigned f = 0; f < 3; f++) if(foldOptions[f] == _folds) option = f;
			_folds = foldOptions[(option + 1) % 3];
			for(unsigned r = 0; r < STREAM_RINGS; r++) // band layout follows the new symmetry
				for(unsigned p = 0; p < _ringCounts[r]; p++)
					Modern_VertexShader::getActorProps()[&_petals[_ringStarts[r] + p]].params[0] = getPetalBand(r, p);
			std::cout << _folds << " fold symmetry" << std::endl;
			break;
		}
		case KEY_6: _spinScale = (_spinScale >= 2.0F)? 0.0F : (_spinScale == 0.0F)? 0.5F : _spinScale * 2.0F; std::cout << "Spin x" << _spinScale << std::endl; break;
		case KEY_7: _isLayered = !_isLayered; std::cout << ((_isLayered)? "Layered tunnel" : "Flat mandala") << std::endl; break;
		case KEY_8: {
			_meshOffset++;
			for(unsigned r = 0; r < STREAM_RINGS; r++)
				for(unsigned p = 0; p < _ringCounts[r]; p++) _petals[_ringStarts[r] + p].setMesh(meshLibrary.getMesh((ringMeshes[r] + _meshOffset) % RUNTIME_MESHES));
			buildScene(&_mandalaScene);
			std::cout << "Petal shapes changed" << std::endl;
			break;
		}
		case KEY_0: for(unsigned r = 0; r < STREAM_RINGS; r++) _ringAngles[r] = 0.0F; break;
		default: break;
	}
}

void Topl_Audiostream::loop(double frameSecs){
	analyze(frameSecs);
	arrange(frameSecs);

	float bass = (_levels[0] + _levels[1] + _levels[2] + _levels[3]) * 0.25F;
	float mids = 0.0F, treble = 0.0F;
	for(unsigned b = 4; b < 10; b++) mids += _levels[b] / 6.0F;
	for(unsigned b = 10; b < STREAM_BANDS; b++) treble += _levels[b] / (STREAM_BANDS - 10);
	_spectrumVShader.setBands(_levels, STREAM_BANDS);
	_spectrumVShader.setSummary(bass, mids, treble, (_loudness + _beat) * 0.5F);

	renderSky();
	renderScene(&_coreScene, MODERN_Spectrum, shaderMode);
	renderScene(&_mandalaScene, MODERN_Spectrum, shaderMode);
}

void Topl_Audiostream::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Audiostream" << std::endl
		<< "  1 microphone   2 next microphone   3 system audio (Windows)   4 synthesized stream" << std::endl
		<< "  5 symmetry folds   6 spin speed   7 flat or layered   8 change petal shapes   0 realign rings" << std::endl
		<< "  M / N shading: neon, lit, monochrome pulse, level heat map. Capture requires SUPPORT_AUDIO" << std::endl << std::endl;
}
