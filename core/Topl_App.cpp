#include "Topl_App.hpp"
 
Topl_Renderer_GL4* Topl_Factory::_renderer_GL4 = nullptr; 
Topl_Renderer_Drx11* Topl_Factory::_renderer_Drx11 = nullptr;
Topl_Pipeline_GL4** Topl_Factory::_pipelines_GL4 = nullptr; 
unsigned Topl_Factory::_pipelineIndex_GL4 = 0;
Topl_Pipeline_Drx11** Topl_Factory::_pipelines_Drx11 = nullptr;
unsigned Topl_Factory::_pipelineIndex_Drx11 = 0;

Topl_Factory::~Topl_Factory(){
    if(_renderer_GL4 != nullptr) delete(_renderer_GL4);
    if(_renderer_Drx11 != nullptr) delete(_renderer_Drx11);

    if(_pipelines_GL4 != nullptr){
        for(unsigned p = 0; p < _pipelineIndex_GL4; p++)
            delete(*(_pipelines_GL4 + p));
        free(_pipelines_GL4);
    }

    if(_pipelines_Drx11 != nullptr){
        for(unsigned p = 0; p < _pipelineIndex_Drx11; p++)
            delete(*(_pipelines_Drx11 + p));
        free(_pipelines_Drx11);
    }
}

Topl_Renderer* Topl_Factory::genRenderer(APP_Backend backend, Platform* platform){
    switch(backend){
    case APP_OpenGL_4:
        if(_renderer_GL4 == nullptr) 
            _renderer_GL4 = new Topl_Renderer_GL4(platform->getParentWindow());
        return (Topl_Renderer*)_renderer_GL4;
    case APP_DirectX_11:
        if(_renderer_Drx11 == nullptr)
            _renderer_Drx11 = new Topl_Renderer_Drx11(platform->getParentWindow());
        return (Topl_Renderer*)_renderer_Drx11;
    }
}

Topl_Pipeline* Topl_Factory::genPipeline(APP_Backend backend, entry_shader_cptr vertexShader, shader_cptr pixelShader){
    // if(_shaders == nullptr) (Topl_Shader**)malloc(MAX_SHADERS * sizeof(Topl_Shader*));
    if(_pipelines_GL4 == nullptr) _pipelines_GL4 = (Topl_Pipeline_GL4**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_GL4*));
    if(_pipelines_Drx11 == nullptr) _pipelines_Drx11 = (Topl_Pipeline_Drx11**)malloc(MAX_PIPELINES * sizeof(Topl_Pipeline_Drx11*));

    switch(backend){
    case APP_OpenGL_4:
        *(_pipelines_GL4 + _pipelineIndex_GL4) = new Topl_Pipeline_GL4();
        _pipelineIndex_GL4++;
        // generate pipeline here
        return *(_pipelines_GL4 + _pipelineIndex_GL4);
    case APP_DirectX_11:
        *(_pipelines_Drx11 + _pipelineIndex_Drx11) = new Topl_Pipeline_Drx11();
        _pipelineIndex_Drx11++;
        // generate pipeline here
        return *(_pipelines_Drx11 + _pipelineIndex_Drx11);
    }
}

Topl_App::Topl_App(const char* execPath, const char* name, APP_Backend backend) : _backend(backend) {
    srand(time(NULL)); // seed random number generation
#ifdef RASTERON_H
	initFreeType(&_freetypeLib);
#endif
		
    _platform = new Platform(execPath, name);
    _platform->createWindow();

    _renderer = Topl_Factory::genRenderer(backend, _platform);
}

Topl_App::~Topl_App() {
    delete(_platform);
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
