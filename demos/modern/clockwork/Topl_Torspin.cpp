#include "Topl_Torspin.hpp"

static const Torspin_Shape shapes[TORSPIN_SHAPES] = { // major radius, tube radius, ribs, rib depth, pipes, panels around the tube
	{ 14.0F, 3.2F, 36.0F, 0.35F, 6.0F, 16.0F },
	{ 22.0F, 4.5F, 60.0F, 0.5F, 8.0F, 20.0F },
	{ 9.0F, 2.4F, 24.0F, 0.3F, 4.0F, 12.0F }
};
static const char* shapeNames[TORSPIN_SHAPES] = { "Ring", "Wide ring", "Tight ring" };

static Vec3f rotateY(const Vec3f& v, float a){ // matches rotateY() inside the modern shaders
	float c = (float)cos(a), s = (float)sin(a);
	return Vec3f({ c * v.data[0] - s * v.data[2], v.data[1], s * v.data[0] + c * v.data[2] });
}

static float wrapAngle(float a){
	while(a > MATH_PI) a -= MATH_PI * 2.0F;
	while(a < -MATH_PI) a += MATH_PI * 2.0F;
	return a;
}

const Torspin_Shape& Topl_Torspin::getShape() const { return shapes[_shape % TORSPIN_SHAPES]; }

Vec3f Topl_Torspin::toTorus(const Vec3f& world) const {
	Vec3f d = world - _anchor;
	Vec3f local = Vec3f({ runtimeDot(_tilt[0], d) + getShape().major, runtimeDot(_tilt[1], d), runtimeDot(_tilt[2], d) });
	return rotateY(local, _spin);
}

Vec3f Topl_Torspin::toWorld(const Vec3f& local) const {
	Vec3f v = rotateY(local, -_spin) - Vec3f({ getShape().major, 0.0F, 0.0F });
	return _anchor + _tilt[0] * v.data[0] + _tilt[1] * v.data[1] + _tilt[2] * v.data[2];
}

void Topl_Torspin::updateTilt(){ // pitch sways while the roll keeps turning, columns of rotZ(roll) * rotX(pitch)
	float pitch = 0.7F * (float)sin(_tiltTime * 0.23F);
	float roll = _tiltTime * 0.17F + 0.4F * (float)sin(_tiltTime * 0.11F);
	Vec3f axes[3] = { Vec3f({ 1.0F, 0.0F, 0.0F }), Vec3f({ 0.0F, (float)cos(pitch), (float)sin(pitch) }), Vec3f({ 0.0F, -(float)sin(pitch), (float)cos(pitch) }) };
	float c = (float)cos(roll), s = (float)sin(roll);
	for(unsigned a = 0; a < 3; a++)
		_tilt[a] = Vec3f({ c * axes[a].data[0] - s * axes[a].data[1], s * axes[a].data[0] + c * axes[a].data[1], axes[a].data[2] });
}

void Topl_Torspin::recenter(){
	const Torspin_Shape& shape = getShape();
	Vec3f d = fps.position - _anchor;
	Vec3f unspun = Vec3f({ runtimeDot(_tilt[0], d) + shape.major, runtimeDot(_tilt[1], d), runtimeDot(_tilt[2], d) });

	// travel along the ring becomes spin, the camera keeps its place in the cross section
	float along = (float)atan2(unspun.data[2], unspun.data[0]);
	unspun = rotateY(unspun, -along);
	_spin += along;

	float radial = unspun.data[0] - shape.major, height = unspun.data[1]; // stay away from the walls
	float offset = (float)sqrt(radial * radial + height * height);
	float limit = (shape.tube - shape.ribDepth) * 0.8F;
	if(offset > limit){
		radial *= limit / offset; height *= limit / offset;
		fps.velocity = fps.velocity * 0.5F;
	}
	Vec3f v = Vec3f({ radial, height, 0.0F });
	fps.position = _anchor + _tilt[0] * v.data[0] + _tilt[1] * v.data[1] + _tilt[2] * v.data[2];
}

void Topl_Torspin::placeLanterns(float t){
	const Torspin_Shape& shape = getShape();
	Vec3f camLocal = toTorus(fps.position);
	float camAngle = (float)atan2(camLocal.data[2], camLocal.data[0]);

	for(unsigned l = 0; l < TORSPIN_LANTERNS; l++){
		float phase = _lanternPhases[l];
		float angle = _lanternAngles[l] + t * 0.04F * (1.0F + phase); // lanterns drift slowly against the walls
		float orbit = t * (0.6F + phase * 0.4F) + phase * 6.0F;
		float ring = shape.major + (float)cos(orbit) * shape.tube * 0.35F;
		Vec3f local = Vec3f({ (float)cos(angle) * ring, (float)sin(orbit) * shape.tube * 0.35F, (float)sin(angle) * ring });
		_lanterns[l].setPos(toWorld(local));

		float pulse = 0.22F + 0.06F * (float)sin(t * 3.0F + phase * 9.0F);
		_lanterns[l].setSize({ pulse, pulse, pulse });
		// the torus curves away, so only lanterns in the visible stretch of the tube are drawn in front of the raymarched walls
		float reach = (float)acos(shape.major / (shape.major + shape.tube)) * 1.6F;
		_lanterns[l].isShown = _isLanterns && (float)fabs(wrapAngle(angle - camAngle)) < reach;
	}
}

void Topl_Torspin::init(){
	if(isGLSL()){ _tunnelVShader = Tunnel_VertexShader_GL4(); _tunnelPShader = Tunnel_PixelShader_GL4(); }
	else { _tunnelVShader = Tunnel_VertexShader_DX11(); _tunnelPShader = Tunnel_PixelShader_DX11(); }
	_tunnelPipeline = genPipeline(&_tunnelVShader, &_tunnelPShader);

	fps.isFlying = true;
	fps.moveSpeed = 3.0F;
	setStart(_anchor, 0.0F, 0.0F); // looking along the tube

	for(unsigned l = 0; l < TORSPIN_LANTERNS; l++){
		_lanternAngles[l] = (MATH_PI * 2.0F / TORSPIN_LANTERNS) * l;
		_lanternPhases[l] = (float)l / TORSPIN_LANTERNS;
		_lanterns[l].setName("torspin_lantern_" + std::to_string(l));
		_lanterns[l].setMesh(&_lanternMesh);
		_lanternScene.addGeometry(&_lanterns[l]);
		float hue = (float)l / TORSPIN_LANTERNS;
		Modern_VertexShader::setActorColor(&_lanterns[l], Modern_Float4(
			0.6F + 0.4F * (float)cos(hue * 6.283F), 0.6F + 0.4F * (float)cos(hue * 6.283F - 2.094F), 0.6F + 0.4F * (float)cos(hue * 6.283F + 2.094F), 1.0F
		));
		Modern_VertexShader::setActorParams(&_lanterns[l], Modern_Float4(32.0F, 0.1F, 1.8F, 1.0F)); // glowing, smooth normals
	}
	updateTilt();
	placeLanterns(0.0F);

	buildScene(&_lanternScene);
	for(unsigned l = 0; l < TORSPIN_LANTERNS; l++) addEditable(&_lanterns[l], &_lanternScene);
}

void Topl_Torspin::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: _spinSpeed *= 0.7F; std::cout << "Spin " << _spinSpeed << " rad/s" << std::endl; break;
		case KEY_2: _spinSpeed *= 1.4F; if(fabs(_spinSpeed) < 0.02F) _spinSpeed = 0.1F; std::cout << "Spin " << _spinSpeed << " rad/s" << std::endl; break;
		case KEY_3: _spinSpeed = -_spinSpeed; std::cout << "Spin reversed" << std::endl; break;
		case KEY_4: _isTilting = !_isTilting; std::cout << "Tilting " << ((_isTilting)? "on" : "off") << std::endl; break;
		case KEY_5: _tiltSpeed *= 0.7F; std::cout << "Tilt speed " << _tiltSpeed << std::endl; break;
		case KEY_6: _tiltSpeed *= 1.4F; std::cout << "Tilt speed " << _tiltSpeed << std::endl; break;
		case KEY_7: _isLanterns = !_isLanterns; break;
		case KEY_8:
			_shape = (_shape + 1) % TORSPIN_SHAPES;
			fps.reset(_anchor, fps.yaw, fps.pitch);
			std::cout << "Shape: " << shapeNames[_shape] << std::endl;
			break;
		case KEY_9: _hueShift += 0.15F; break;
		case KEY_0:
			_spin = 0.0F; _spinSpeed = 0.3F; _tiltTime = 0.0F; _tiltSpeed = 1.0F; _isTilting = true; _hueShift = 0.0F;
			fps.reset(_anchor, 0.0F, 0.0F);
			break;
		default: break;
	}
}

void Topl_Torspin::loop(double frameSecs){
	const Torspin_Shape& shape = getShape();
	float step = (isPaused)? 0.0F : (float)(frameSecs * timeScale);
	_spin += _spinSpeed * step;
	if(_isTilting) _tiltTime += _tiltSpeed * step;
	updateTilt();

	recenter();
	fps.apply(&camera);
	placeLanterns((float)elapsed);

	_tunnelVShader.setFrame(_tilt, _spin, shape.major, shape.tube, _anchor, shape.ribs);
	_tunnelVShader.setDetail(shape.ribDepth, shape.pipes, 1.0F, shape.panels);
	_tunnelVShader.setLights(2.0F + (float)fabs(_spinSpeed) * 4.0F, 0.25F, _hueShift, 2.2F);

	renderFullscreen(_tunnelPipeline, &_tunnelVShader, shaderMode);
	renderScene(&_lanternScene, MODERN_Lighting, (shaderMode == 3)? 2 : 0);
}

void Topl_Torspin::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Torspin" << std::endl
		<< "  1 / 2 slower / faster spin   3 reverse spin   4 tilting on / off   5 / 6 slower / faster tilt" << std::endl
		<< "  7 lanterns   8 next torus shape   9 shift colors   0 reset" << std::endl
		<< "  M / N shading: industrial, neon, organic, normals. Moving along the tube turns the torus under you" << std::endl << std::endl;
}
