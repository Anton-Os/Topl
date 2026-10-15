#include "Topl_Visualizer.hpp"

static const Modern_Float4 groupColors[VISUALIZER_OBJECTS] = {
	Modern_Float4(0.85F, 0.25F, 0.2F, 1.0F), Modern_Float4(0.9F, 0.75F, 0.35F, 1.0F), Modern_Float4(0.3F, 0.55F, 0.95F, 1.0F), // lit
	Modern_Float4(1.0F, 0.55F, 0.25F, 1.0F), Modern_Float4(0.45F, 0.85F, 0.5F, 1.0F), Modern_Float4(0.75F, 0.45F, 0.95F, 1.0F), // toon
	Modern_Float4(0.2F, 0.9F, 1.0F, 0.8F), Modern_Float4(0.35F, 1.0F, 0.6F, 0.8F), Modern_Float4(0.9F, 0.4F, 1.0F, 0.8F) // hologram
};

MODERN_Pipeline Topl_Visualizer::getGroupPipeline(unsigned group) const {
	switch(_view){
		case VISUALIZER_Lighting: return MODERN_Lighting;
		case VISUALIZER_Toon: return MODERN_Toon;
		case VISUALIZER_Hologram: return MODERN_Hologram;
		default: return (group == 0)? MODERN_Lighting : (group == 1)? MODERN_Toon : MODERN_Hologram;
	}
}

void Topl_Visualizer::arrangeGallery(){ // places objects on a ring and lifts them above the terrain
	for(unsigned o = 0; o < VISUALIZER_OBJECTS; o++){
		float angle = (MATH_PI * 2.0F / VISUALIZER_OBJECTS) * o;
		float x = sin(angle) * VISUALIZER_RING;
		float z = cos(angle) * VISUALIZER_RING + 4.0F;
		float y = _terrainVShader.getGroundHeight(x, z) + 1.8F;
		_basePositions[o] = Vec3f({ x, y, z });
		_objects[o].setPos(_basePositions[o]);
		Modern_VertexShader::setActorColor(&_objects[o], groupColors[o]);
	}

	for(unsigned o = 0; o < 3; o++){
		float smooth = (o == 0)? 1.0F : 0.0F; // the first object of each group is an orb with smooth sphere normals
		Modern_VertexShader::setActorParams(&_objects[o], Modern_Float4(24.0F + o * 40.0F, o * 0.45F, (o == 2)? 0.25F : 0.0F, smooth)); // shininess, reflectivity, glow
		Modern_VertexShader::setActorParams(&_objects[3 + o], Modern_Float4(0.0F, 0.0F, 0.0F, smooth));
		Modern_VertexShader::setActorParams(&_objects[6 + o], Modern_Float4(1.0F + o * 0.5F, 0.0F, 0.0F, smooth)); // glitch strength
	}

	// mesh extents differ (orbs reach 0.7, tori 1.5), so sizes are evened out
	for(unsigned g = 0; g < 3; g++){
		_objects[g * 3].setSize({ 1.4F, 1.4F, 1.4F });
		if(g != 1) _objects[g * 3 + 2].setSize({ 0.65F, 0.65F, 0.65F });
	}
}

void Topl_Visualizer::init(){
	_terrainVShader.setTerrain(-1.2F, 9.0F, 0.045F);
	fps.ground = [this](float x, float z){ return _terrainVShader.getGroundHeight(x, z); };
	fps.isFlying = false;

	float startY = _terrainVShader.getGroundHeight(0.0F, -6.0F) + fps.eyeHeight;
	setStart(Vec3f({ 0.0F, startY, -6.0F }), 0.0F, -0.08F);

	arrangeGallery();

	buildScene(&_terrainScene);
	for(unsigned g = 0; g < 3; g++) buildScene(&_groupScenes[g]);
	for(unsigned o = 0; o < VISUALIZER_OBJECTS; o++) addEditable(&_objects[o], &_groupScenes[o / 3]); // menu panes and picking
}

void Topl_Visualizer::onPipelineSelect(unsigned index){
	static const RUNTIME_Key viewKeys[5] = { KEY_0, KEY_1, KEY_2, KEY_3, KEY_4 };
	if(index < 5) onPress(viewKeys[index]);
	else {
		_terrainMode = (int)(index - 5) % RUNTIME_MODES;
		std::cout << "Terrain mode " << _terrainMode << std::endl;
	}
}

void Topl_Visualizer::onPress(RUNTIME_Key key){
	VISUALIZER_View lastView = _view;
	switch(key){
		case KEY_0: _view = VISUALIZER_Gallery; break;
		case KEY_1: _view = VISUALIZER_Lighting; break;
		case KEY_2: _view = VISUALIZER_Toon; break;
		case KEY_3: _view = VISUALIZER_Hologram; break;
		case KEY_4: _view = (_view == VISUALIZER_Raymarch)? VISUALIZER_Gallery : VISUALIZER_Raymarch; break;
		case KEY_5:
			_terrainMode = (_terrainMode + 1) % RUNTIME_MODES;
			std::cout << "Terrain mode " << _terrainMode << std::endl;
			break;
		default: break;
	}

	if(_view != lastView){
		static const char* viewNames[5] = { "Gallery", "Lighting", "Toon", "Hologram", "Raymarch" };
		std::cout << "View: " << viewNames[_view] << std::endl;
	}
}

void Topl_Visualizer::loop(double frameSecs){
	float t = (float)elapsed;

	renderSky();

	if(_view == VISUALIZER_Raymarch){ // the raymarcher draws its own world, the FPS camera still drives it
		renderFullscreen(MODERN_Raymarch, shaderMode);
		return;
	}

	// keep the terrain centered under the camera, snapped to the grid spacing so vertices never swim
	float spacing = _terrainMesh.getSpacing();
	float camX = fps.position.data[0], camZ = fps.position.data[2];
	_terrainActor.setPos({ (float)floor(camX / spacing) * spacing, 0.0F, (float)floor(camZ / spacing) * spacing });
	renderScene(&_terrainScene, MODERN_Terrain, _terrainMode);

	for(unsigned o = 0; o < VISUALIZER_OBJECTS; o++){
		float phase = (float)o * 0.7F;
		_objects[o].setPos(_basePositions[o] + Vec3f({ 0.0F, (float)sin(t * 1.3F + phase) * 0.35F, 0.0F }));
		_objects[o].setRot({ t * 0.3F + phase, t * 0.5F, t * 0.7F + phase });
	}

	for(unsigned g = 0; g < 3; g++) renderScene(&_groupScenes[g], getGroupPipeline(g), shaderMode);
}

void Topl_Visualizer::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Visualizer" << std::endl
		<< "  0 gallery   1 lighting   2 toon   3 hologram   4 raymarch world   5 terrain mode" << std::endl << std::endl;
}
