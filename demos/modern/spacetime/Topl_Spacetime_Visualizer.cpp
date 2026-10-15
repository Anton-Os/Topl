#include "Topl_Spacetime_Visualizer.hpp"

void Topl_Spacetime_Visualizer::updateBodies(){
	float t = (float)elapsed;

	for(unsigned b = 0; b <= SPACETIME_PLANETS; b++){
		Spacetime_Body& body = _bodies[b];
		float x = 0.0F, z = 0.0F, mass = body.mass;

		if(b == 0 && _isBinary){ // the star becomes two lighter stars circling each other
			x = cos(t * 1.6F) * 2.5F;
			z = sin(t * 1.6F) * 2.5F;
			mass = body.mass * 0.5F;
		} else if(b > 0){
			x = cos(t * body.orbitSpeed + body.phase) * body.orbitRadius;
			z = sin(t * body.orbitSpeed + body.phase) * body.orbitRadius;
		}
		if(b > _visiblePlanets) mass = 0.0F;

		// with the binary option the slot of the outer planet carries the second star
		if(b == SPACETIME_PLANETS && _isBinary){
			x = -cos(t * 1.6F) * 2.5F;
			z = -sin(t * 1.6F) * 2.5F;
			mass = _bodies[0].mass * 0.5F;
		}

		_spacetimeVShader.setMass(b, Vec3f({ x, 0.0F, z }), mass);
	}
	_spacetimeVShader.setField(_gravity, 1.4F, (_isWaves)? 0.12F : 0.0F, 1.1F);

	for(unsigned b = 0; b <= SPACETIME_PLANETS; b++){ // bodies rest inside their own wells
		Modern_Float4 massData = _spacetimeVShader.getParam(b);
		bool isStar = b == 0 || (b == SPACETIME_PLANETS && _isBinary);
		float size = (isStar)? _bodies[0].size * ((_isBinary)? 0.7F : 1.0F) : _bodies[b].size;
		float y = -_spacetimeVShader.getWell(massData[0], massData[2]) + size * 0.6F;

		_bodyActors[b].isShown = massData[3] > 0.0F;
		_bodyActors[b].setPos({ massData[0], y, massData[2] });
		_bodyActors[b].setSize({ size, size, size });
		_bodyActors[b].setRot({ 0.0F, 0.0F, t * 0.4F });
		Modern_VertexShader::setActorColor(&_bodyActors[b], (isStar)? Modern_Float4(1.0F, 0.75F, 0.35F, 1.0F) : Modern_Float4(0.35F + b * 0.15F, 0.55F, 1.0F - b * 0.2F, 1.0F));
		Modern_VertexShader::setActorParams(&_bodyActors[b], Modern_Float4(64.0F, 0.3F, (isStar)? 2.5F : 0.0F, 1.0F)); // smooth sphere normals
	}
}

void Topl_Spacetime_Visualizer::init(){
	for(unsigned b = 1; b <= SPACETIME_PLANETS; b++) // speeds follow Kepler's third law
		_bodies[b].orbitSpeed = 3.2F / (float)pow(_bodies[b].orbitRadius, 1.5F) * 3.0F;

	setEnvironment(2); // night sky
	fps.isFlying = true;
	setStart(Vec3f({ 0.0F, 10.0F, -30.0F }), 0.0F, -0.3F);

	Modern_VertexShader::setActorParams(&_fabricActor, Modern_Float4(1.0F, 0.0F, 0.0F, 0.0F)); // grid cell size
	updateBodies();

	buildScene(&_fabricScene);
	buildScene(&_bodyScene);
	for(unsigned b = 0; b <= SPACETIME_PLANETS; b++) addEditable(&_bodyActors[b], &_bodyScene); // menu panes and picking
}

void Topl_Spacetime_Visualizer::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: _isBinary = !_isBinary; std::cout << "Binary star " << ((_isBinary)? "on" : "off") << std::endl; break;
		case KEY_2: _gravity *= 1.25F; std::cout << "Gravity " << _gravity << std::endl; break;
		case KEY_3: _gravity /= 1.25F; std::cout << "Gravity " << _gravity << std::endl; break;
		case KEY_4: _isWaves = !_isWaves; std::cout << "Gravitational waves " << ((_isWaves)? "on" : "off") << std::endl; break;
		case KEY_5: _visiblePlanets = (_visiblePlanets + 1) % (SPACETIME_PLANETS + 1); std::cout << "Planets " << _visiblePlanets << std::endl; break;
		default: break;
	}
}

void Topl_Spacetime_Visualizer::loop(double frameSecs){
	updateBodies();

	renderSky();
	renderScene(&_fabricScene, MODERN_Spacetime, shaderMode);
	renderScene(&_bodyScene, MODERN_Lighting, 0);
}

void Topl_Spacetime_Visualizer::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Spacetime" << std::endl
		<< "  1 binary star   2 / 3 gravity up / down   4 gravitational waves   5 planet count" << std::endl << std::endl;
}
