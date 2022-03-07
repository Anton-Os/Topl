#include "Topl_App.hpp"

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
    srand(time(NULL)); // seed random number generation
#ifdef RASTERON_H
		initFreeType(&_freetypeLib);
#endif
		
    _platform = new Platform(execPath, name);
    _platform->createWindow();

    switch (backend) {
    case APP_OpenGL_4:
        _renderer_GL4 = new Topl_Renderer_GL4(_platform->getParentWindow());
        _renderer = (Topl_Renderer*)_renderer_GL4;

        _pipelines_GL4 = (Topl_Pipeline_GL4**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_GL4*));
        break;
    case APP_DirectX_11:
        _renderer_Drx11 = new Topl_Renderer_Drx11(_platform->getParentWindow());
        _renderer = (Topl_Renderer*)_renderer_Drx11;

        _pipelines_Drx11 = (Topl_Pipeline_Drx11**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Drx11*));
        break;
    }
}

Topl_App::~Topl_App() {
    delete(_platform);

    switch (_backend) {
    case APP_OpenGL_4:
        delete _renderer_GL4;

        for(unsigned p = 0; p < _pipelineIndex; p++)
            delete(*(_pipelines_GL4 + p));
        free(_pipelines_GL4);
        break;
    case APP_DirectX_11:
        delete _renderer_Drx11;

        for(unsigned p = 0; p < _pipelineIndex; p++)
            delete(*(_pipelines_Drx11 + p));
        free(_pipelines_Drx11);
        break;
    }
#ifdef RASTERON_H
	cleanupFreeType(&_freetypeLib);
#endif
}

void Topl_App::run(){
    init();

    while (1) { 
        _ticker.updateTimer();
        _renderer->clearView();

        loop(_ticker.getAbsSecs(), _renderer->getFrameCount());
        
        _renderer->switchFramebuff();
        _platform->handleEvents(false); // TODO: handle cursor updates periodically
    }
}

Topl_Pipeline* Topl_App::genPipeline(const std::string& vertexSource, const std::string& pixelSource){
    Topl_Pipeline* pipeline = nullptr;

    switch (_backend) {
    case APP_OpenGL_4:
        *(_pipelines_GL4 + _pipelineIndex) = new Topl_Pipeline_GL4();
        // Create shaders, compile and link pipeline
        break;
    case APP_DirectX_11:
        *(_pipelines_Drx11 + _pipelineIndex) = new Topl_Pipeline_Drx11();
        // Create shaders, compile and link pipeline
        break;
    }
    _pipelineIndex++;

    return pipeline;
}
