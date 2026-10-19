#include "Topl_Planetos.hpp"

#include <new>

static const char* biomeNames[PLANETOS_BIOMES] = { "Earthlike", "Desert", "Ice", "Volcanic" };
static const float evolveSpeeds[4] = { 0.0F, 1.0F, 6.0F, 25.0F }; // terrain time per second
static const float daySpeeds[4] = { 0.0F, 0.02F, 0.08F, 0.3F }; // radians per second
static const float seaLevels[4] = { -2.0F, 0.4F, 2.5F, 5.0F };

static float smoothStep(float a, float b, float x){ float t = (x - a) / (b - a); t = (t < 0.0F)? 0.0F : (t > 1.0F)? 1.0F : t; return t * t * (3.0F - 2.0F * t); }
static float mixf(float a, float b, float t){ return a + (b - a) * t; }

// Sphere

Planetos_Sphere::Planetos_Sphere(unsigned short divisions) : Geo_Mesh() {
	unsigned divs = (divisions > 0)? divisions : 1;
	unsigned row = divs + 1;
	unsigned vertexCount = row * row * 6;
	unsigned indexCount = divs * divs * 6 * 6;

	static const float faces[6][3][3] = { // normal, u axis, v axis for each cube face
		{ { 1, 0, 0 }, { 0, 0, -1 }, { 0, 1, 0 } }, { { -1, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 } },
		{ { 0, 1, 0 }, { 1, 0, 0 }, { 0, 0, -1 } }, { { 0, -1, 0 }, { 1, 0, 0 }, { 0, 0, 1 } },
		{ { 0, 0, 1 }, { 1, 0, 0 }, { 0, 1, 0 } }, { { 0, 0, -1 }, { -1, 0, 0 }, { 0, 1, 0 } }
	};

	// populate() copies and then frees both arrays, so they are allocated with malloc
	Geo_Vertex* vertices = (Geo_Vertex*)malloc(sizeof(Geo_Vertex) * vertexCount);
	unsigned* indices = (unsigned*)malloc(sizeof(unsigned) * indexCount);

	unsigned v = 0, i = 0;
	for(unsigned f = 0; f < 6; f++){
		unsigned base = v;
		for(unsigned y = 0; y < row; y++)
			for(unsigned x = 0; x < row; x++){
				float s = (float)x / divs * 2.0F - 1.0F, t = (float)y / divs * 2.0F - 1.0F; // exact on shared edges
				Vec3f p = Vec3f({
					faces[f][0][0] + faces[f][1][0] * s + faces[f][2][0] * t,
					faces[f][0][1] + faces[f][1][1] * s + faces[f][2][1] * t,
					faces[f][0][2] + faces[f][1][2] * s + faces[f][2][2] * t
				});
				p = runtimeNormalize(p);
				new (&vertices[v++]) Geo_Vertex(p, Vec3f({ (float)x / divs, (float)y / divs, (float)f }), p, Vec3f({ 1.0F, 1.0F, 1.0F }));
			}
		for(unsigned y = 0; y < divs; y++)
			for(unsigned x = 0; x < divs; x++){
				unsigned a = base + y * row + x, b = a + 1, c = a + row, d = c + 1;
				indices[i++] = a; indices[i++] = c; indices[i++] = b;
				indices[i++] = b; indices[i++] = c; indices[i++] = d;
			}
	}

	populate(vertexCount, vertices, indexCount, indices);
}

// Planet frame

Vec3f Topl_Planetos::toWorld(const Vec3f& dir) const { return Vec3f({ runtimeDot(_rows[0], dir), runtimeDot(_rows[1], dir), runtimeDot(_rows[2], dir) }); }

Vec3f Topl_Planetos::toPlanet(const Vec3f& w) const { return _rows[0] * w.data[0] + _rows[1] * w.data[1] + _rows[2] * w.data[2]; }

float Topl_Planetos::getSurface(const Vec3f& dir) const {
	float h = _planetVShader.getTerrain(dir.data);
	return (h > _seaLevel)? h : _seaLevel;
}

float Topl_Planetos::getGround(float x, float z) const {
	Vec3f worldDir = runtimeNormalize(Vec3f({ x, PLANETOS_RADIUS, z })); // direction from the center through the point
	float r = PLANETOS_RADIUS + getSurface(toPlanet(worldDir));
	return _center.data[1] + worldDir.data[1] * r;
}

void Topl_Planetos::orthonormalize(){ // keeps the accumulated rotation from drifting
	Vec3f cols[3];
	for(unsigned c = 0; c < 3; c++) cols[c] = Vec3f({ _rows[0].data[c], _rows[1].data[c], _rows[2].data[c] });
	cols[0] = runtimeNormalize(cols[0]);
	cols[1] = runtimeNormalize(cols[1] - cols[0] * runtimeDot(cols[0], cols[1]));
	cols[2] = runtimeCross(cols[0], cols[1]);
	for(unsigned r = 0; r < 3; r++) _rows[r] = Vec3f({ cols[0].data[r], cols[1].data[r], cols[2].data[r] });
}

void Topl_Planetos::travel(){
	float dx = fps.position.data[0], dz = fps.position.data[2];
	float dist = (float)sqrt(dx * dx + dz * dz);
	if(dist < 1e-6F) return;

	// walking toward a point on the surface rotates that point up to the top of the planet
	Vec3f heading = Vec3f({ dx / dist, 0.0F, dz / dist });
	Vec3f axis = runtimeCross(heading, Vec3f({ 0.0F, 1.0F, 0.0F }));
	float altitude = PLANETOS_RADIUS + fps.position.data[1];
	float angle = dist / ((altitude > PLANETOS_RADIUS * 0.5F)? altitude : PLANETOS_RADIUS * 0.5F);

	Vec3f cols[3];
	for(unsigned c = 0; c < 3; c++) cols[c] = runtimeRotate(Vec3f({ _rows[0].data[c], _rows[1].data[c], _rows[2].data[c] }), axis, angle);
	for(unsigned r = 0; r < 3; r++) _rows[r] = Vec3f({ cols[0].data[r], cols[1].data[r], cols[2].data[r] });
	orthonormalize();

	fps.position.data[0] = 0.0F;
	fps.position.data[2] = 0.0F;
	if(!fps.isFlying){ // settle onto the surface at the new spot
		float ground = getGround(0.0F, 0.0F) + fps.eyeHeight;
		if(fps.position.data[1] < ground) fps.position.data[1] = ground;
	}
}

void Topl_Planetos::updateSky(){ // the sun circles the planet, so walking around it moves through day and night
	Modern_SceneState& state = Modern_VertexShader::getState();
	Vec3f sunDir = toWorld(runtimeNormalize(Vec3f({ (float)cos(_dayAngle), 0.35F, (float)sin(_dayAngle) })));
	float day = smoothStep(-0.25F, 0.2F, sunDir.data[1]);
	float dusk = smoothStep(-0.3F, 0.0F, sunDir.data[1]) * (1.0F - smoothStep(0.05F, 0.35F, sunDir.data[1]));
	Vec3f lightDir = (day > 0.05F)? sunDir : sunDir * -1.0F; // the moon lights the night side
	float altitude = fps.position.data[1];

	state.lightPos = Modern_Float4(lightDir.data[0], lightDir.data[1], lightDir.data[2], -mixf(0.35F, 1.1F, day));
	state.lightColor = Modern_Float4(
		mixf(0.4F, mixf(1.0F, 1.0F, dusk), day), mixf(0.48F, mixf(0.95F, 0.6F, dusk), day), mixf(0.75F, mixf(0.85F, 0.4F, dusk), day),
		mixf(0.22F, 0.5F, day)
	);
	state.fog = Modern_Float4(
		mixf(0.04F, mixf(0.62F, 0.9F, dusk), day), mixf(0.06F, mixf(0.72F, 0.55F, dusk), day), mixf(0.12F, mixf(0.85F, 0.45F, dusk), day),
		0.0045F / (1.0F + ((altitude > 0.0F)? altitude : 0.0F) * 0.03F) // thinner haze when high above the surface
	);
}

void Topl_Planetos::placeGems(){
	float t = (float)elapsed;
	for(unsigned g = 0; g < PLANETOS_GEMS; g++){
		float hover = 2.6F + 0.4F * (float)sin(t * 1.3F + g);
		Vec3f worldDir = toWorld(_gemDirs[g]);
		_gems[g].setPos(_center + worldDir * (PLANETOS_RADIUS + getSurface(_gemDirs[g]) + hover));
		_gems[g].setRot({ 0.0F, 0.0F, t * 0.8F + g });
	}
}

// Demo

void Topl_Planetos::init(){
	if(isGLSL()){ _planetVShader = Planet_VertexShader_GL4(); _planetPShader = Planet_PixelShader_GL4(); }
	else { _planetVShader = Planet_VertexShader_DX11(); _planetPShader = Planet_PixelShader_DX11(); }
	_planetPipeline = genPipeline(&_planetVShader, &_planetPShader);

	_planetVShader.setPlanet(_rows, PLANETOS_RADIUS, _heightScale, _seaLevel);
	_planetVShader.setTerrain(_terrainTime, 1.6F);
	_planetVShader.setPulse(-1.0F, 0.0F, _pulseOrigin);
	_planetVShader.setBiome(_biome);
	_planetVShader.setCenter(_center, 1.0F);

	fps.isFlying = false;
	fps.ground = [this](float x, float z){ return getGround(x, z); };
	setStart(Vec3f({ 0.0F, getGround(0.0F, 0.0F) + fps.eyeHeight, 0.0F }), 0.0F, -0.05F);

	srand(7); // the same landmarks every launch
	for(unsigned g = 0; g < PLANETOS_GEMS; g++){
		Vec3f dir;
		do { dir.randomize(-1.0F, 1.0F); } while(dir.len() < 0.1F || dir.len() > 1.0F);
		_gemDirs[g] = runtimeNormalize(dir);
		if(g == 0) _gemDirs[g] = runtimeNormalize(Vec3f({ 0.08F, 1.0F, 0.12F })); // one gem just ahead of the start
		_gems[g].setName("planetos_gem_" + std::to_string(g));
		_gems[g].setMesh(&_gemMesh);
		_gemScene.addGeometry(&_gems[g]);
		float hue = (float)g / PLANETOS_GEMS;
		Modern_VertexShader::setActorColor(&_gems[g], Modern_Float4(
			0.6F + 0.4F * (float)cos(hue * 6.283F), 0.6F + 0.4F * (float)cos(hue * 6.283F - 2.094F), 0.6F + 0.4F * (float)cos(hue * 6.283F + 2.094F), 1.0F
		));
		Modern_VertexShader::setActorParams(&_gems[g], Modern_Float4(96.0F, 0.4F, 0.6F, 0.0F)); // shiny with a soft glow
	}
	placeGems();

	buildScene(&_planetScene);
	buildScene(&_gemScene);
	for(unsigned g = 0; g < PLANETOS_GEMS; g++) addEditable(&_gems[g], &_gemScene);
}

void Topl_Planetos::onPipelineSelect(unsigned index){
	if(index < PLANETOS_BIOMES) onPress((RUNTIME_Key)(KEY_1 + index));
	else shaderMode = (int)(index - PLANETOS_BIOMES) % RUNTIME_MODES;
}

void Topl_Planetos::onPress(RUNTIME_Key key){
	switch(key){
		case KEY_1: case KEY_2: case KEY_3: case KEY_4:
			_biome = (unsigned)(key - KEY_1);
			std::cout << "Biome: " << biomeNames[_biome] << std::endl;
			break;
		case KEY_5: // quake ring starting under the camera
			_pulseAge = 0.0F;
			_pulseOrigin = toPlanet(Vec3f({ 0.0F, 1.0F, 0.0F }));
			std::cout << "Quake!" << std::endl;
			break;
		case KEY_6:
			_evolveSetting = (_evolveSetting + 1) % 4;
			std::cout << "Terrain evolution x" << evolveSpeeds[_evolveSetting] << std::endl;
			break;
		case KEY_7: _isTides = !_isTides; std::cout << "Tides " << ((_isTides)? "on" : "off") << std::endl; break;
		case KEY_8:
			_daySetting = (_daySetting + 1) % 4;
			std::cout << "Day cycle speed " << daySpeeds[_daySetting] << std::endl;
			break;
		case KEY_9: {
			unsigned level = 0;
			for(unsigned l = 0; l < 4; l++) if(seaLevels[l] == _baseSea) level = l;
			_baseSea = seaLevels[(level + 1) % 4];
			std::cout << "Sea level " << _baseSea << std::endl;
			break;
		}
		case KEY_0:
			_isOrbit = !_isOrbit;
			fps.isFlying = _isOrbit;
			fps.velocity = Vec3f({ 0.0F, 0.0F, 0.0F });
			if(_isOrbit){ fps.position.data[1] = PLANETOS_RADIUS * 1.25F; fps.pitch = -1.2F; }
			else { fps.position.data[1] = getGround(0.0F, 0.0F) + fps.eyeHeight; fps.pitch = -0.05F; }
			std::cout << ((_isOrbit)? "Orbit view" : "Surface view") << std::endl;
			break;
		default: break;
	}
}

void Topl_Planetos::loop(double frameSecs){
	float step = (isPaused)? 0.0F : (float)(frameSecs * timeScale);
	_terrainTime += step * evolveSpeeds[_evolveSetting] * 0.05F;
	_dayAngle += step * daySpeeds[_daySetting];
	_seaLevel = _baseSea + ((_isTides)? 0.6F * (float)sin(elapsed * 0.25) : 0.0F);
	if(_pulseAge >= 0.0F){
		_pulseAge += step;
		if(_pulseAge > 30.0F) _pulseAge = -1.0F;
	}

	float altitude = (fps.position.data[1] > 0.0F)? fps.position.data[1] : 0.0F;
	fps.moveSpeed = 6.0F + altitude * 0.25F; // faster when high above the surface

	_planetVShader.setPlanet(_rows, PLANETOS_RADIUS, _heightScale, _seaLevel);
	_planetVShader.setTerrain(_terrainTime, 1.6F);
	_planetVShader.setPulse(_pulseAge, 3.5F, _pulseOrigin);
	_planetVShader.setBiome(_biome);
	_planetVShader.setCenter(_center, 1.0F);

	travel(); // the terrain below uses the values set above
	fps.apply(&camera);
	_planetVShader.setPlanet(_rows, PLANETOS_RADIUS, _heightScale, _seaLevel);
	updateSky();
	placeGems();

	renderSky();
	renderScene(&_planetScene, _planetPipeline, &_planetVShader, shaderMode);
	renderScene(&_gemScene, MODERN_Lighting, (shaderMode == 2)? 2 : 0);
}

void Topl_Planetos::printControls() const {
	Topl_Runtime::printControls();
	std::cout
		<< "Planetos" << std::endl
		<< "  1 earthlike   2 desert   3 ice   4 volcanic   5 quake under you   6 terrain evolution speed" << std::endl
		<< "  7 tides   8 day cycle speed   9 sea level   0 orbit or surface view" << std::endl
		<< "  M / N shading: natural, contours, normals, height map. Walk any direction to circle the planet" << std::endl << std::endl;
}
