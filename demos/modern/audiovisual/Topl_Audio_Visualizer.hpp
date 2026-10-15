#include "../Topl_Runtime.hpp"

// Audio Visualizer: a ring of spectrum bars, a pulsing orb and a rippling floor driven by live audio analysis
// With SUPPORT_AUDIO enabled the runtime plays the selected track and analyzes it in sync with playback,
// otherwise a synthesized spectrum keeps the scene moving

#define AUDIO_BANDS SPECTRUM_BANDS
#define AUDIO_RING 7.5F
#define AUDIO_BAR_HEIGHT 6.0F
#define AUDIO_MIN_FREQ 40.0F
#define AUDIO_MAX_FREQ 8000.0F

class Topl_Audio_Visualizer : public Topl_Runtime {
public:
	Topl_Audio_Visualizer(const char* execPath) : Topl_Runtime(execPath, "Audiovisual"){ }
	Topl_Audio_Visualizer(const char* execPath, BACKEND_Target backend) : Topl_Runtime(execPath, "Audiovisual", backend){ }
protected:
	void init() override;
	void loop(double frameSecs) override;
	void onPress(RUNTIME_Key key) override;
	bool isMaterialEditable() const override { return false; } // actor params carry band indices and kinds
	void printControls() const override;
private:
	void analyze(double frameSecs); // fills _levels with smoothed band levels
	void synthesize(float* levels); // procedural spectrum used without audio

	float _levels[AUDIO_BANDS] = { 0.0F };
	float _peaks[AUDIO_BANDS] = { 0.0F };
	Vec3f _barPositions[AUDIO_BANDS];

	Geo_Quad3D _barMesh = Geo_Quad3D(0.5F, 0.5F);
	Geo_Actor _bars[AUDIO_BANDS];
	Topl_Scene _barScene = Topl_Scene(&camera);

	Geo_Orb _orbMesh = Geo_Orb({ 1.0F, 64, 64 });
	Geo_Actor _orbActor = Geo_Actor("audio_orb", &_orbMesh);
	Runtime_Grid _floorMesh = Runtime_Grid(90.0F, 160);
	Geo_Actor _floorActor = Geo_Actor("audio_floor", &_floorMesh);
	Topl_Scene _stageScene = Topl_Scene(&camera, { &_orbActor, &_floorActor });
};
