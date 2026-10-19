#include "../Topl_Runtime.hpp"

#include <atomic>

// Audiostream: live audio captured from a microphone (or the system output on Windows) drives a mandala of shapes.
// Rings of petals follow the spectrum from bass in the center to treble on the rim, mirrored across the mandala folds,
// beats flash the rings and send them spinning. Without SUPPORT_AUDIO or a capture device a synthesized stream plays

#define STREAM_BANDS SPECTRUM_BANDS
#define STREAM_RINGS 5
#define STREAM_PETALS 144 // 12 + 24 + 24 + 36 + 48, every ring count divides by 4, 6 and 12
#define STREAM_RING_BUFFER 16384 // samples kept from the capture thread, a power of two
#define STREAM_WINDOW 2048 // samples analyzed per frame
#define STREAM_MIN_FREQ 40.0F
#define STREAM_MAX_FREQ 8000.0F

enum STREAM_Source { STREAM_Synth = 0, STREAM_Microphone = 1, STREAM_Loopback = 2 };

// Capture through a miniaudio device, the device thread fills a ring buffer that the frame loop reads

class Stream_Capture {
public:
	~Stream_Capture(){ stop(); close(); }

	bool start(STREAM_Source source, unsigned deviceIndex); // device index applies to microphones
	void stop();
	bool isActive() const { return _isDevice; }
	unsigned getDeviceCount();
	std::string getDeviceName(unsigned deviceIndex);
	float getSampleRate() const { return _sampleRate; }
	bool read(float* samples, unsigned count) const; // most recent samples, false without a device
private:
	void close();

	std::vector<float> _ring = std::vector<float>(STREAM_RING_BUFFER, 0.0F);
	std::atomic<unsigned long long> _written = { 0 };
	float _sampleRate = 48000.0F;
	bool _isContext = false, _isDevice = false;
#ifdef TOPL_ENABLE_AUDIO
	static void onData(ma_device* device, void* output, const void* input, ma_uint32 frameCount);

	ma_context _context;
	ma_device _device;
	ma_device_info* _captureInfos = nullptr;
	ma_uint32 _captureCount = 0;
#endif
};

class Topl_Audiostream : public Topl_Runtime {
public:
	Topl_Audiostream(const char* execPath) : Topl_Runtime(execPath, "Audiostream"){ }
	Topl_Audiostream(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Audiostream", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	bool isMaterialEditable() const override { return false; } // actor params carry band indices
	void printControls() const override;
private:
	void setSource(STREAM_Source source);
	void analyze(double frameSecs); // band levels, loudness and beats from the stream
	void synthesize(float* levels);
	void arrange(double frameSecs); // positions every petal from the current levels
	float getPetalBand(unsigned ring, unsigned petal) const; // band index mirrored across the folds

	Stream_Capture _capture;
	STREAM_Source _source = STREAM_Synth;
	unsigned _deviceIndex = 0;
	std::vector<float> _window = std::vector<float>(STREAM_WINDOW, 0.0F);

	float _levels[STREAM_BANDS] = { 0.0F };
	float _floors[STREAM_BANDS] = { 0.0F }; // adaptive noise floor per band
	float _loudness = 0.0F, _bassAverage = 0.0F, _beat = 0.0F, _beatCooldown = 0.0F;
	unsigned _folds = 6;
	unsigned _meshOffset = 0;
	bool _isLayered = false; // rings spread into depth like a tunnel
	float _spinScale = 1.0F;
	float _ringAngles[STREAM_RINGS] = { 0.0F };

	// Mandala
	unsigned _ringCounts[STREAM_RINGS] = { 12, 24, 24, 36, 48 };
	unsigned _ringStarts[STREAM_RINGS] = { 0 };
	Geo_Actor _petals[STREAM_PETALS];
	Topl_Scene _mandalaScene = Topl_Scene(&camera);
	Geo_Orb _coreMesh = Geo_Orb({ 1.0F, 48, 48 });
	Geo_Actor _coreActor = Geo_Actor("stream_core", &_coreMesh);
	Geo_Torus _haloMesh = Geo_Torus(0.06F);
	Geo_Actor _halos[2] = { Geo_Actor("stream_halo_inner", &_haloMesh), Geo_Actor("stream_halo_outer", &_haloMesh) };
	Topl_Scene _coreScene = Topl_Scene(&camera, { &_coreActor, &_halos[0], &_halos[1] });
};
