#include <iostream>
#include <thread>
#include <functional>  // for std::ref

#include "Hello.hpp"

// #define TARGET_BACKEND BACKEND_GL4
// #define TARGET_BACKEND BACKEND_DX11
// #define TARGET_BACKEND BACKEND_VK
#define TARGET_BACKEND TOPL_DEFAULT_BACKEND // DirectX on Windows, OpenGL elsewhere

#define FRAME_AVG_TIME 100
#define FRAME_SPIKE_TIME 20

#define ACTOR_COUNT 1

static Geo_Trig2D triangle;
static Geo_Actor actor = Geo_Actor((Geo_Mesh*)&triangle);
std::vector<Vec3f> calcPoints(1, VEC_3F_ONES);

static Topl_Scene scene = Topl_Scene();

#ifdef TOPL_ENABLE_AUDIO
static SdTrack* audioTrack = nullptr;
#endif

static bool background_input_call(std::string& input) {
	std::getline(std::cin, input);
	std::cout << "Input received from thread: " << input << std::endl;
	return true;
}

// Hello Main Loop

static void logFrameRate(double f1, double f2, double f3, double f4){
    static unsigned frameCount = 0;
    static double frameTotal = 0.0;

    frameCount++; 
    frameTotal += f1 + f2 + f3 + f4;

    std::cout << "Frame times: "
        << f1 << " on start, " << f2 << " on clear, " << f3 << " on render, " << f4 << " on present, "
        << " Total: " << f1 + f2 + f3 + f4
        << " | Average: " << frameTotal / frameCount << std::endl;
}

static int audioPlaybackTest(std::string audioFileName) {
#ifdef TOPL_ENABLE_AUDIO // Audio Playback Support
	std::string soundFilePath = std::string(AUDIO_DIR) + audioFileName;
	audioTrack = SdTrack_open(soundFilePath.c_str());
	if (audioTrack == nullptr) return -1; // error

	static unsigned audioFrameCount = 4096;
	static std::vector<float> audioData(audioFrameCount, 0.0);
	if (SdTrack_read(audioTrack, 0, audioData.data(), audioFrameCount) == 0) return -2; // error
	if (!SdTrack_play(audioTrack, false)) return -3; // error
#endif
	return 0;
}

MAIN_ENTRY {
	std::cout << "Creating platform" << std::endl;
#ifndef __ANDROID__
	Platform platform(argv[0], "Hello");
    std::cout << "Creating window" << std::endl;
	platform.createWindow(TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT);
#else
    Platform platform(pApp);
    while(platform.getParentWindow() == nullptr && platform.handleEvents() && !pApp->destroyRequested)
        platform.awaitWindow(); // waiting for window on Android
#endif
    std::cout << "Creating backend" << std::endl;
	BACKEND_Target backendTarget = TARGET_BACKEND;
	Topl_Renderer* renderer = nullptr;
	if(backendTarget == BACKEND_GL4) renderer = new Hello_Renderer_GL4(platform.getContext());
#ifdef _WIN32
	else if(backendTarget == BACKEND_DX11) renderer = new Hello_Renderer_DX11(platform.getContext());
#endif
#if defined(TOPL_ENABLE_VULKAN) && !defined(__ANDROID__)
	else if(backendTarget == BACKEND_VK) renderer = new Hello_Renderer_VK(platform.getContext());
#endif
	
	if (renderer == nullptr) return -1; // renderer is null

	Timer_Persist _ticker;
	double frameTotal = 0.0;
	double swapTime = 0.0;

	std::cout << "Scene building" << std::endl;
	triangle.setInstances({ Mat4x4::scale({ 1.5F, 1.5F, 1.5F }), Mat4x4::scale({ 2.0F, 2.0F, 2.0F }), Mat4x4::scale({ 2.5F, 2.5F, 2.5F }), Mat4x4::scale({ 3.0F, 3.0F, 3.0F }) });
	scene.addGeometry(&actor);
	renderer->buildScene(&scene);
	
	std::thread backgroundThread;
	std::string commandArgs = "";
#ifdef TOPL_ENABLE_AUDIO
	audioPlaybackTest("200hz-sine-freqies.mp3");
#endif
	std::cout << "Rendering loop" << std::endl;
	while (platform.handleEvents()) {
		if (!backgroundThread.joinable() && commandArgs.empty()) {
			commandArgs = "PLACEHOLDER";
			// backgroundThread = std::thread(background_empty_call);
			backgroundThread = std::thread(background_input_call, std::ref(commandArgs));
		}
		// if (backgroundThread.joinable()) backgroundThread.detach();

		double f1 = _ticker.getRelMillisecs();
		renderer->clear();
		double f2 = _ticker.getRelMillisecs();
#if !defined(__ANDROID__) && TARGET_BACKEND!=BACKEND_VK
		renderer->setDrawPipeline(false);
		renderer->dispatch(&calcPoints);
		renderer->setDrawPipeline(true);
#endif
		renderer->update(&actor);
		renderer->draw(&actor);
		double f3 = _ticker.getRelMillisecs();
		renderer->present();
		double f4 = _ticker.getRelMillisecs();
		// logFrameRate(f1, f2, f3, f4);
	}

#ifdef TOPL_ENABLE_AUDIO
	SdTrack_close(audioTrack); // Audio Cleanup
#endif
    if(renderer != nullptr) delete(renderer);
	// return 0;
}
