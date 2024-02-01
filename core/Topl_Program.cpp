#include "Topl_Program.hpp"

// Timeline

std::map<Vec3f*, std::map<millisec_t, Vec3f>> Topl_Timeline::vec3f_map = std::map<Vec3f*, std::map<millisec_t, Vec3f>>();
std::map<float*, std::map<millisec_t, float>> Topl_Timeline::float_map = std::map<float*, std::map<millisec_t, float>>();
std::map<double*, std::map<millisec_t, double>> Topl_Timeline::double_map = std::map<double*, std::map<millisec_t, double>>();

Topl_Timeline::Topl_Timeline(){
	// dynamic_ticker.addRecurringEvent(Topl_Timeline::seqCallback);
}

void Topl_Timeline::seqCallback(millisec_t m){
	for(auto f = Topl_Timeline::vec3f_map.begin(); f != vec3f_map.end(); f++){
		std::map<millisec_t, Vec3f>::iterator seq_start = f->second.begin();
		std::map<millisec_t, Vec3f>::reverse_iterator seq_end = f->second.rbegin();

		if(f->second.size() > 2){ // std::cout << "2+ range detected" << std::endl;
			auto b = std::next(seq_start);
			while(b != f->second.end()) if(m / MILLISEC_IN_SEC > b->first){ seq_start = b; b++; } else break;
			auto t = std::next(seq_end);
			while(t != f->second.rend()) if(m / MILLISEC_IN_SEC < t->first){ seq_end = t; t++; } else break;
		}

		double prog =  ((m - seq_start->first) / MILLISEC_IN_SEC) / (seq_end->first - seq_start->first);

		if(m / MILLISEC_IN_SEC > seq_end->first) *(f->first) = seq_end->second; // went over the time limit
		else if(m / MILLISEC_IN_SEC < seq_start->first) *(f->first) = seq_start->second;
		else *(f->first) = seq_start->second + ((seq_end->second - seq_start->second) * prog);
		// else *(f->first) = r->second + ((m - r->first) * ((s->second - r->second) / (s->first - r->first)));
	}

	for(auto f = Topl_Timeline::float_map.begin(); f != float_map.end(); f++){
		std::map<millisec_t, float>::iterator seq_start = f->second.begin();
		std::map<millisec_t, float>::reverse_iterator seq_end = f->second.rbegin();

		if(f->second.size() > 2){ // std::cout << "2+ range detected" << std::endl;
			auto b = std::next(seq_start);
			while(b != f->second.end()) if(m / MILLISEC_IN_SEC > b->first){ seq_start = b; b++; } else break;
			auto t = std::next(seq_end);
			while(t != f->second.rend()) if(m / MILLISEC_IN_SEC < t->first){ seq_end = t; t++; } else break;
		}

		double prog =  ((m - seq_start->first) / MILLISEC_IN_SEC) / (seq_end->first - seq_start->first);

		if(m / MILLISEC_IN_SEC > seq_end->first) *(f->first) = seq_end->second; // went over the time limit
		else if(m / MILLISEC_IN_SEC < seq_start->first) *(f->first) = seq_start->second;
		else *(f->first) = seq_start->second + ((seq_end->second - seq_start->second) * prog);
		// else *(f->first) = r->second + ((m - r->first) * ((s->second - r->second) / (s->first - r->first)));
	}

	for(auto f = Topl_Timeline::double_map.begin(); f != double_map.end(); f++){
		std::map<millisec_t, double>::iterator seq_start = f->second.begin();
		std::map<millisec_t, double>::reverse_iterator seq_end = f->second.rbegin();

		if(f->second.size() > 2){ // std::cout << "2+ range detected" << std::endl;
			auto b = std::next(seq_start);
			while(b != f->second.end()) if(m / MILLISEC_IN_SEC > b->first){ seq_start = b; b++; } else break;
			auto t = std::next(seq_end);
			while(t != f->second.rend()) if(m / MILLISEC_IN_SEC < t->first){ seq_end = t; t++; } else break;
		}

		double prog =  ((m - seq_start->first) / MILLISEC_IN_SEC) / (seq_end->first - seq_start->first);

		if(m / MILLISEC_IN_SEC > seq_end->first) *(f->first) = seq_end->second; // went over the time limit
		else if(m / MILLISEC_IN_SEC < seq_start->first) *(f->first) = seq_start->second;
		else *(f->first) = seq_start->second + ((seq_end->second - seq_start->second) * prog);
		// else *(f->first) = r->second + ((m - r->first) * ((s->second - r->second) / (s->first - r->first)));
	}
}

void Topl_Timeline::addSequence_vec3f(Vec3f* var, std::pair<millisec_t, Vec3f> target){
	auto sequence = std::find_if(Topl_Timeline::vec3f_map.begin(), Topl_Timeline::vec3f_map.end(), [var](const std::pair<Vec3f*, std::map<millisec_t, Vec3f>>& p){ return p.first == var; });
	
	if(sequence != Topl_Timeline::vec3f_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::vec3f_map.insert({ var, std::map<millisec_t, Vec3f>() }); // create object
		if(target.first != 0.0) Topl_Timeline::vec3f_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::vec3f_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

void Topl_Timeline::addSequence_float(float* var, std::pair<millisec_t, float> target){
	auto sequence = std::find_if(Topl_Timeline::float_map.begin(), Topl_Timeline::float_map.end(), [var](const std::pair<float*, std::map<millisec_t, float>>& p){ return p.first == var; });
	
	if(sequence != Topl_Timeline::float_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::float_map.insert({ var, std::map<millisec_t, float>() }); // create object
		if(target.first != 0.0) Topl_Timeline::float_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::float_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

void Topl_Timeline::addSequence_double(double* var, std::pair<millisec_t, double> target){
	auto sequence = std::find_if(Topl_Timeline::double_map.begin(), Topl_Timeline::double_map.end(), [var](const std::pair<double*, std::map<millisec_t, double>>& p){ return p.first == var; });
	
	if(sequence != Topl_Timeline::double_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::double_map.insert({ var, std::map<millisec_t, double>() }); // create object
		if(target.first != 0.0) Topl_Timeline::double_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::double_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

// Program

Topl_Timeline Topl_Program::timeline = Topl_Timeline();

Vec3f Topl_Program::cursorPos = { 0.0F, 0.0F, 0.0F };
bool Topl_Program::isCamera_KeyControl = true;
bool Topl_Program::isInputEnabled = false;
std::string Topl_Program::userInput = "";

#ifdef RASTERON_H
unsigned Topl_Program::pickerVal_color = NO_COLOR;
unsigned Topl_Program::pickerVal_coord = NO_COLOR;
const Geo_Actor* Topl_Program::pickerObj = NO_PICKER_OBJ;
Rasteron_Queue* Topl_Program::cachedFrames = NULL;
#endif

Topl_Camera Topl_Program::cameraObj = Topl_Camera();


static void onAnyKey(char k){
	if(isalnum(k) || isspace(k)){
		Topl_Program::userInput += k;
		std::cout << Topl_Program::userInput << std::endl;
	}

	if(Topl_Program::isCamera_KeyControl){
		if(toupper(k) == 'W') Topl_Program::cameraObj.updatePos({ 0.0, 0.1F, 0.0 });
		else if(toupper(k) == 'S') Topl_Program::cameraObj.updatePos({ 0.0, -0.1F, 0.0 });
		else if(toupper(k) == 'A') Topl_Program::cameraObj.updatePos({ -0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'D') Topl_Program::cameraObj.updatePos({ 0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'Q') Topl_Program::cameraObj.updateRot({ -0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'E') Topl_Program::cameraObj.updateRot({ 0.1F, 0.0, 0.0 });
		else if(toupper(k) == 'Z') Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 1.1);
		else if(toupper(k) == 'C') Topl_Program::cameraObj.setZoom(*Topl_Program::cameraObj.getZoom() * 0.9);
	}
}

static void onPress(float x, float y){ 
	if(!Topl_Program::isInputEnabled) Topl_Program::userInput.clear();
	Topl_Program::cursorPos = { x, y, 0.0F }; 
}

Topl_Program::Topl_Program(const char* execPath, const char* name, BACKEND_Target backend) : _backend(backend) {
    _platform = new Platform(execPath, name);
	_platform->createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
    _renderer = Topl_Factory::genRenderer(backend, _platform);
	_renderer->setCamera(&Topl_Program::cameraObj);
	_renderer->setDrawMode(DRAW_Triangles);

	Platform::keyControl.addAnyCallback(onAnyKey);

	Platform::mouseControl.addCallback(MOUSE_LeftBtn_Down, onPress);
    // Platform::mouseControl.addCallback(MOUSE_RightBtn_Down, onPress);
	// Platform::mouseControl.addHoverCallback(onPress);
#ifdef RASTERON_H
	_invertImage = INVERT_IMG_TRUE; // make sure images show up inverse

	// ImageSize frameSize = { Platform::getViewportHeight(_platformCtx.window), Platform::getViewportWidth(_platformCtx.window) };
	ImageSize frameSize = { TOPL_WIN_HEIGHT, TOPL_WIN_WIDTH };
	Topl_Program::cachedFrames = alloc_queue("frames", frameSize, CACHED_FRAME_COUNT);
#endif
}

void Topl_Program::cleanup() {
#ifdef RASTERON_H
	for(unsigned f = 0; f < Topl_Program::cachedFrames->frameCount; f++){ // exports frames
		std::string frameName = "frame" + std::to_string(f + 1) + ".bmp";
		Rasteron_Image* frameImg = getFrameAt(Topl_Program::cachedFrames, f);
		writeFileImageRaw(frameName.c_str(), IMG_Bmp, frameImg->height, frameImg->width, frameImg->data);
	}
	if(Topl_Program::cachedFrames != NULL) dealloc_queue(Topl_Program::cachedFrames);
	cleanupFreeType();
#endif
	delete(_renderer);
	delete(_platform);
}

void Topl_Program::run(){
    init();

    while (_platform->handleEvents()) {
		Topl_Timeline::seqCallback(Topl_Program::timeline.dynamic_ticker.getAbsMillisecs());

		_renderer->clear(); // clears view to solid color
		loop(Topl_Program::timeline.persist_ticker.getRelMillisecs()); // performs draws and updating
		_renderer->present(); // switches front and back buffers
#ifdef RASTERON_H
		Img_Base frameImg = _renderer->frame();
		addFrameAt(Topl_Program::cachedFrames, frameImg.getImage(), _renderer->getFrameCount() % CACHED_FRAME_COUNT);
#endif
	}

	cleanup();
}


#ifdef RASTERON_H
unsigned Topl_Program::colorPicker(Topl_Scene* scene){
	Topl_Program::pickerVal_color = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	if((Topl_Program::pickerVal_color & 0x00FFFFFF) == (CLEAR_COLOR_CODE & 0x00FFFFFF)) 
		Topl_Program::pickerObj = nullptr;
	if(scene != nullptr){ 
		const Geo_Actor* actor = scene->getPickActor(Topl_Program::pickerVal_color);
		if(actor != nullptr) Topl_Program::pickerObj = actor; 
	}
	return Topl_Program::pickerVal_color;
}

Vec3f Topl_Program::coordPicker(Topl_Scene* scene){
	Topl_Program::pickerVal_coord = _renderer->getPixelAt(Platform::getCursorX(), Platform::getCursorY());
	return Vec3f{
		((Topl_Program::pickerVal_coord & 0xFF0000) >> 16) / 255.0f,
		((Topl_Program::pickerVal_coord & 0xFF00) >> 8) / 255.0f, 
		(Topl_Program::pickerVal_coord & 0xFF) / 255.0f,  
	};
}
#endif