#include "Topl_Fractile.hpp"

static const Fractile_Preset presets[FRACTAL_COUNT] = {
	{ "Mandelbulb", 8.0F, 8, 4.0F, 1.25F, 0.8F, Vec3f({ 0.0F, 1.0F, -11.0F }) },
	{ "Menger sponge", 0.0F, 5, 4.0F, 1.75F, 0.5F, Vec3f({ 1.5F, 1.2F, -13.0F }) },
	{ "Sierpinski tetrahedron", 0.0F, 12, 4.0F, 1.9F, 0.6F, Vec3f({ 0.0F, 2.0F, -13.0F }) },
	{ "Mandelbox", -1.8F, 12, 0.75F, 7.0F, 0.6F, Vec3f({ 0.0F, 0.5F, -12.0F }) },
	{ "Quaternion julia", 0.0F, 11, 4.0F, 1.6F, 0.9F, Vec3f({ 0.0F, 0.6F, -10.0F }) }
};
static const float detailLevels[3] = { 2.0F, 1.0F, 0.5F }; // epsilon factors, smaller is finer

void Topl_Fractile::selectFractal(unsigned index){
	_fractal = (FRACTAL_Type)(index % FRACTAL_COUNT);
	_iterations = presets[_fractal].iterations;
	setStart(presets[_fractal].start, 0.0F, -0.05F);
	std::cout << "Fractal: " << presets[_fractal].name << std::endl;
}

float Topl_Fractile::getSurfaceDistance() const {
	const Fractile_Preset& preset = presets[_fractal];
	float core = preset.worldScale * preset.bound * 0.55F; // most detail lives inside this radius
	float dist = fps.position.len() - core;
	return (dist > preset.worldScale * 0.02F)? dist : preset.worldScale * 0.02F;
}

void Topl_Fractile::init(){
	if(isGLSL()){ _fractalVShader = Fractal_VertexShader_GL4(); _fractalPShader = Fractal_PixelShader_GL4(); }
	else { _fractalVShader = Fractal_VertexShader_DX11(); _fractalPShader = Fractal_PixelShader_DX11(); }
	_fractalPipeline = genPipeline(&_fractalVShader, &_fractalPShader);

	setEnvironment(2); // night sky keeps the colors of the fractal in front
	fps.isFlying = true;
	selectFractal(FRACTAL_Mandelbulb);
}

void Topl_Fractile::onPipelineSelect(unsigned index){
	if(index < FRACTAL_COUNT) selectFractal(index);
	else shaderMode = (int)(index - FRACTAL_COUNT) % RUNTIME_MODES;
}

void Topl_Fractile::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: case KEY_2: case KEY_3: case KEY_4: case KEY_5: selectFractal((unsigned)(key - KEY_1)); break;
		case KEY_6:
			_isMorphing = !_isMorphing;
			std::cout << "Morphing " << ((_isMorphing)? "on" : "off") << std::endl;
			break;
		case KEY_7: if(_iterations > 1) _iterations--; std::cout << "Iterations " << _iterations << std::endl; break;
		case KEY_8: if(_iterations < 24) _iterations++; std::cout << "Iterations " << _iterations << std::endl; break;
		case KEY_9:
			_detailLevel = (_detailLevel + 1) % 3;
			std::cout << "Detail " << ((_detailLevel == 0)? "coarse" : (_detailLevel == 1)? "normal" : "fine") << std::endl;
			break;
		case KEY_0:
			_isSpinning = !_isSpinning;
			std::cout << "Spin " << ((_isSpinning)? "on" : "off") << std::endl;
			break;
		default: break;
	}
}

void Topl_Fractile::loop(double frameSecs){
	const Fractile_Preset& preset = presets[_fractal];
	if(_isMorphing && !isPaused) _morphTime += frameSecs * timeScale;
	if(_isSpinning && !isPaused) _spin += (float)(frameSecs * timeScale) * 0.15F;
	float t = (float)_morphTime;

	fps.moveSpeed = getSurfaceDistance() * 0.9F; // slows down near the surface so small details stay reachable
	if(fps.moveSpeed > 8.0F) fps.moveSpeed = 8.0F;

	float shape = preset.shape, fold = 0.0F;
	switch(_fractal){
		case FRACTAL_Mandelbulb: shape = 8.0F + 3.0F * (float)sin(t * 0.12F); break; // power sweeps between 5 and 11
		case FRACTAL_Menger: fold = 0.08F * (float)sin(t * 0.1F); break;
		case FRACTAL_Sierpinski: fold = 0.25F * (float)sin(t * 0.15F); break;
		case FRACTAL_Mandelbox: shape = -1.8F + 0.25F * (float)sin(t * 0.08F); break;
		default: break;
	}
	_fractalVShader.setFractal(_fractal, shape, _iterations, preset.worldScale, preset.bound);
	_fractalVShader.setLook(_colorShift + t * 0.01F, preset.glow, detailLevels[_detailLevel]);
	_fractalVShader.setMotion(_spin, 0.0F, 0.85F, fold);
	_fractalVShader.setJulia(Modern_Float4( // orbits a region of interesting julia sets
		-0.2F + 0.25F * (float)sin(t * 0.11F), 0.6F * (float)cos(t * 0.07F),
		0.2F * (float)sin(t * 0.09F + 1.0F), 0.15F * (float)cos(t * 0.05F)
	));

	renderFullscreen(_fractalPipeline, &_fractalVShader, shaderMode);
}

void Topl_Fractile::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Fractile" << std::endl
		<< "  1 mandelbulb   2 menger sponge   3 sierpinski   4 mandelbox   5 quaternion julia" << std::endl
		<< "  6 morphing on / off   7 / 8 fewer / more iterations   9 detail   0 spin" << std::endl
		<< "  M / N shading: shaded, neon orbit trap, normals, step count. Movement slows near the surface" << std::endl << std::endl;
}
