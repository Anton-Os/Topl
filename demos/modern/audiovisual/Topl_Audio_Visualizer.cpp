#include "Topl_Audio_Visualizer.hpp"

// Audio

void Topl_Audio_Visualizer::synthesize(float* levels){
	float t = (float)elapsed;
	float beat = (float)pow(fabs(sin(t * MATH_PI * 2.0F)), 8.0F); // 120 bpm kick
	for(unsigned b = 0; b < AUDIO_BANDS; b++){
		float f = (float)b / AUDIO_BANDS;
		float melody = 0.5F + 0.5F * (float)sin(t * (1.3F + f * 3.0F) + b * 0.9F);
		float shimmer = 0.5F + 0.5F * (float)sin(t * 7.0F + b * 2.3F);
		float target = beat * (1.0F - f) * 0.9F + melody * 0.45F * (0.4F + f) + shimmer * 0.15F * f;
		levels[b] = (target < 1.0F)? target : 1.0F;
	}
}

void Topl_Audio_Visualizer::analyze(double frameSecs){
	float raw[AUDIO_BANDS];
	if(!audio.analyze(raw, AUDIO_BANDS, AUDIO_MIN_FREQ, AUDIO_MAX_FREQ)) synthesize(raw); // live audio when a track plays

	float decay = (float)exp(-frameSecs * 6.0); // fast attack, smooth release
	for(unsigned b = 0; b < AUDIO_BANDS; b++){
		_peaks[b] = (raw[b] > _peaks[b] * decay)? raw[b] : _peaks[b] * decay;
		_levels[b] = _peaks[b];
	}
}

// Demo

void Topl_Audio_Visualizer::init(){
	setEnvironment(3); // aurora night
	fps.isFlying = true;
	setStart(Vec3f({ 0.0F, 4.0F, -16.0F }), 0.0F, -0.15F);

	for(unsigned b = 0; b < AUDIO_BANDS; b++){
		float angle = (MATH_PI * 2.0F / AUDIO_BANDS) * b;
		_barPositions[b] = Vec3f({ (float)sin(angle) * AUDIO_RING, 0.0F, (float)cos(angle) * AUDIO_RING });
		_bars[b].setName("audio_bar_" + std::to_string(b));
		_bars[b].setMesh(&_barMesh);
		_bars[b].setPos(_barPositions[b]);
		_bars[b].setRot({ 0.0F, 0.0F, angle });
		_barScene.addGeometry(&_bars[b]);
		Modern_VertexShader::setActorParams(&_bars[b], Modern_Float4((float)b, 0.0F, 0.0F, 0.0F)); // band index, bar kind
	}

	_orbActor.setPos({ 0.0F, 3.0F, 0.0F });
	_orbActor.setSize({ 2.6F, 2.6F, 2.6F });
	Modern_VertexShader::setActorParams(&_orbActor, Modern_Float4(0.0F, 2.0F, 0.0F, 0.0F)); // orb kind
	_floorActor.setPos({ 0.0F, -0.01F, 0.0F });
	Modern_VertexShader::setActorParams(&_floorActor, Modern_Float4(0.0F, 1.0F, 0.0F, 0.0F)); // floor kind
	Modern_VertexShader::setActorTexture(&_floorActor, Modern_Float4(0.0F, 1.0F, 0.0F, 1.0F)); // world mapped once textured

	if(!audio.play(0)) std::cout << "Audio unavailable, using a synthesized spectrum (enable SUPPORT_AUDIO for playback)" << std::endl;

	buildScene(&_barScene);
	buildScene(&_stageScene);
	for(unsigned b = 0; b < AUDIO_BANDS; b++) addEditable(&_bars[b], &_barScene); // menu panes and picking
	addEditable(&_orbActor, &_stageScene);
}

void Topl_Audio_Visualizer::onPress(RUNTIME_Key key){
	if(key == KEY_0){
		audio.stop();
		std::cout << "Synthesized spectrum" << std::endl;
	} else if(key >= KEY_1 && key <= KEY_9) audio.play((unsigned)(key - KEY_1));
}

void Topl_Audio_Visualizer::loop(double frameSecs){
	analyze(frameSecs);

	float bass = 0.0F, mids = 0.0F, treble = 0.0F;
	for(unsigned b = 0; b < AUDIO_BANDS; b++){
		if(b < 4) bass += _levels[b] * 0.25F;
		else if(b < 10) mids += _levels[b] / 6.0F;
		else treble += _levels[b] / 6.0F;
	}
	_spectrumVShader.setBands(_levels, AUDIO_BANDS);
	_spectrumVShader.setSummary(bass, mids, treble, (bass + mids + treble) / 3.0F);

	for(unsigned b = 0; b < AUDIO_BANDS; b++){ // bars grow upward from the floor, whatever mesh they use
		float height = 0.15F + _levels[b] * AUDIO_BAR_HEIGHT;
		_bars[b].setSize({ 1.0F, height / getMeshHeight(_bars[b].getMesh()), 1.0F });
		_bars[b].setPos(_barPositions[b] + Vec3f({ 0.0F, height * 0.5F, 0.0F }));
	}
	_orbActor.setRot({ 0.0F, (float)elapsed * 0.2F, (float)elapsed * 0.35F });

	renderSky();
	renderScene(&_stageScene, MODERN_Spectrum, shaderMode);
	renderScene(&_barScene, MODERN_Spectrum, shaderMode);
}

void Topl_Audio_Visualizer::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Audiovisual" << std::endl
		<< "  1 - 9 play test tones (requires SUPPORT_AUDIO)   0 synthesized spectrum" << std::endl << std::endl;
}
