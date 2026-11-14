#include "Topl_Audiostream.hpp"

static const char* sourceNames[3] = { "Synthesized stream", "Microphone", "System audio (loopback)" };
static const unsigned ringMeshes[STREAM_RINGS] = { 8, 6, 5, 1, 3 }; // gem, cone, triangle prism, cube, hex prism
static const float ringRadii[STREAM_RINGS] = { 2.0F, 3.4F, 4.8F, 6.4F, 8.2F };
static const float ringSizes[STREAM_RINGS] = { 0.42F, 0.38F, 0.4F, 0.36F, 0.34F };
static const unsigned foldOptions[3] = { 4, 6, 12 };

// Analysis

#ifdef TOPL_ENABLE_AUDIO
Topl_Audiostream::~Topl_Audiostream(){ SdStream_free(_stream); } // stops the capture device

void Topl_Audiostream::analyze(double frameSecs){
	if(SdStream_isActive(_stream) && SdStream_read(_stream, _window.data(), STREAM_WINDOW))
		SdBands_process(&_bands, _window.data(), STREAM_WINDOW, SdStream_getSampleRate(_stream), frameSecs); // rooms and microphones differ, so levels sit above the noise floor
	else SdBands_synthesize(&_bands, elapsed, frameSecs);

	for(unsigned b = 0; b < STREAM_BANDS; b++) _levels[b] = _bands.levels[b]; // fast attack, smooth release
	_bass = _bands.bass; _mids = _bands.mids; _treble = _bands.treble;
	_loudness = _bands.loudness;
	_beat = _bands.beat;
}
#else
Topl_Audiostream::~Topl_Audiostream(){ }

static float clamp01(float x){ return (x < 0.0F)? 0.0F : (x > 1.0F)? 1.0F : x; }

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

void Topl_Audiostream::analyze(double frameSecs){ // without Soundscape the stream is always synthesized
	float raw[STREAM_BANDS];
	synthesize(raw);
	_loudness = 0.0F;
	for(unsigned b = 0; b < STREAM_BANDS; b++) _loudness += raw[b] / STREAM_BANDS;

	float decay = (float)exp(-frameSecs * 6.0); // fast attack, smooth release
	for(unsigned b = 0; b < STREAM_BANDS; b++) _levels[b] = (raw[b] > _levels[b] * decay)? raw[b] : _levels[b] * decay;

	_bass = 0.0F; _mids = 0.0F; _treble = 0.0F;
	for(unsigned b = 0; b < STREAM_BANDS; b++){
		if(b < 4) _bass += _levels[b] * 0.25F;
		else if(b < 10) _mids += _levels[b] / 6.0F;
		else _treble += _levels[b] / (STREAM_BANDS - 10);
	}

	_bassAverage += (_bass - _bassAverage) * (1.0F - (float)exp(-frameSecs * 2.0));
	_beatCooldown -= (float)frameSecs;
	if(_bass > _bassAverage * 1.3F + 0.08F && _beatCooldown <= 0.0F){ _beat = 1.0F; _beatCooldown = 0.22F; }
	_beat *= (float)exp(-frameSecs * 5.0);
}
#endif

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

	float coreSize = 1.2F + _bass * 0.7F + _beat * 0.3F;
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
	bool isLive = false;
#ifdef TOPL_ENABLE_AUDIO
	isLive = (source != STREAM_Synth) && SdStream_start(_stream, (SD_StreamSource)source, _deviceIndex); // device index applies to microphones
	if(source == STREAM_Synth) SdStream_stop(_stream);
	SdBands_setAdaptive(&_bands, true); // noise floors start over for the new source
#endif

	if(isLive){
#ifdef TOPL_ENABLE_AUDIO
		std::cout << "Streaming from " << sourceNames[source];
		if(source == STREAM_Microphone) std::cout << ": " << SdStream_getDeviceName(_stream, _deviceIndex);
		std::cout << " at " << (int)SdStream_getSampleRate(_stream) << " Hz" << std::endl;
#endif
	} else {
		if(source != STREAM_Synth){
			std::cout << sourceNames[source] << " is unavailable";
			if(source == STREAM_Loopback) std::cout << " (loopback needs WASAPI on Windows)";
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

#ifdef TOPL_ENABLE_AUDIO
	SdBands_init(&_bands, STREAM_BANDS, STREAM_MIN_FREQ, STREAM_MAX_FREQ);
#endif
	setSource(STREAM_Microphone); // falls back to the synthesized stream without a capture device
}

void Topl_Audiostream::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: setSource(STREAM_Microphone); break;
		case KEY_2: {
			unsigned count = 0;
#ifdef TOPL_ENABLE_AUDIO
			count = SdStream_getDeviceCount(_stream);
#endif
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

	_spectrumVShader.setBands(_levels, STREAM_BANDS);
	_spectrumVShader.setSummary(_bass, _mids, _treble, (_loudness + _beat) * 0.5F);

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
