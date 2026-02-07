#include "program/Topl_Program.hpp"

void Topl_Program::setPipelines(){
    if(_backend == BACKEND_GL4){
        _texVShader = Textured_VertexShader_GL4(); _texPShader = Textured_PixelShader_GL4();
        _beamsVShader = Beams_VertexShader_GL4(); _beamsPShader = Beams_PixelShader_GL4();
        _materialVShader = Material_VertexShader_GL4(); _materialPShader = Material_PixelShader_GL4();
        _effectVShader = Effect_VertexShader_GL4(); _effectPShader = Effect_PixelShader_GL4();
        _canvasVShader = Canvas_VertexShader_GL4(); _canvasPShader = Canvas_PixelShader_GL4();
        _fieldVShader = Field_VertexShader_GL4(); _fieldPShader = Field_PixelShader_GL4();
        _patternVShader = Pattern_VertexShader_GL4(); _patternPShader = Pattern_PixelShader_GL4();
        _flatVShader = Flat_VertexShader_GL4(); _flatPShader = Flat_PixelShader_GL4();
        for(unsigned p = 0; p < PROGRAM_PIPELINES; p++){
            _geomShaders[p] = Advance_GeometryShader_GL4();
            _tessCtrlShaders[p] = Advance_TessCtrlShader_GL4();
            _tessEvalShaders[p] = Advance_TessEvalShader_GL4();
        }
    }
#ifdef _WIN32
    else if(_backend == BACKEND_DX11){
        _texVShader = Textured_VertexShader_DX11(); _texPShader = Textured_PixelShader_DX11();
        _beamsVShader = Beams_VertexShader_DX11(); _beamsPShader = Beams_PixelShader_DX11();
        _materialVShader = Material_VertexShader_DX11(); _materialPShader = Material_PixelShader_DX11();
        _effectVShader = Effect_VertexShader_DX11(); _effectPShader = Effect_PixelShader_DX11();
        _canvasVShader = Canvas_VertexShader_DX11(); _canvasPShader = Canvas_PixelShader_DX11();
        _fieldVShader = Field_VertexShader_DX11(); _fieldPShader = Field_PixelShader_DX11();
        _patternVShader = Pattern_VertexShader_DX11(); _patternPShader = Pattern_PixelShader_DX11();
        _flatVShader = Flat_VertexShader_DX11(); _flatPShader = Flat_PixelShader_DX11();
        for(unsigned p = 0; p < PROGRAM_PIPELINES; p++){
            _geomShaders[p] = Advance_GeometryShader_DX11();
            _tessCtrlShaders[p] = Advance_TessCtrlShader_DX11();
            _tessEvalShaders[p] = Advance_TessEvalShader_DX11();
        }
    }
#endif
    _flatPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader);
    _texPipeline = Topl_Factory::genPipeline(_backend, &_texVShader, &_texPShader);
    _beamsPipeline = Topl_Factory::genPipeline(_backend, &_beamsVShader, &_beamsPShader);
    _materialPipeline = Topl_Factory::genPipeline(_backend, &_materialVShader, &_materialPShader);
    _canvasPipeline = Topl_Factory::genPipeline(_backend, &_canvasVShader, &_canvasPShader);
    _fieldPipeline = Topl_Factory::genPipeline(_backend, &_fieldVShader, &_fieldPShader);
    _patternPipeline = Topl_Factory::genPipeline(_backend, &_patternVShader, &_patternPShader);
    _effectPipeline = Topl_Factory::genPipeline(_backend, &_effectVShader, &_effectPShader); // TODO: Figure out why this fails
#ifndef __linux__!
    _geomPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader, { &_geomShaders[0] }); // flat shader for now
    _tessPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader, { &_tessCtrlShaders[0], &_tessEvalShaders[0] }); // flat shader for now
    _longPipeline = Topl_Factory::genPipeline(_backend, &_flatVShader, &_flatPShader, { &_geomShaders[0], &_tessCtrlShaders[0], &_tessEvalShaders[0] }); // flat shader for now
#endif
    Topl_Factory::switchPipeline(_renderer, _flatPipeline); // _texPipeline);
}

void Topl_Program::updatePipelines(){
    float timeElapse = (float)timeline.dynamic_ticker.getAbsSecs();
    Vec3f scroll = (*camera.getPos() * -Topl_Program::speed); // + Vec3f({ sin((*camera.getRot)[0]), cos((*camera.getRot)[0]), 0.0F }); // TODO: Include rotation
    Vec3f scale = Vec3f({ 0.5F, 0.5F, 0.5F }) * ((1.0F / *(camera.getZoom())) * 0.5F); // (1.0F / ((*camera.getZoom() + 4.0F) * 0.1F));
    Input_TracerStep tracerStep = (Platform::mouseControl.getTracerSteps()->size() > 0)? (*Platform::mouseControl.getTracerSteps()).back() : Input_TracerStep();

    if(_renderer->getPipeline() == _texPipeline) _texVShader.setParams(&_background.actor, { 0, (timeElapse / 10) - floor(timeElapse / 10), scroll, scale });
    else if(_renderer->getPipeline() == _materialPipeline){
        _materialVShader.setTexCoordParams(scroll, scale);
        _materialVShader.setLight(Topl_Light(Vec3f({ sin(timeElapse), cos(timeElapse), sin(timeElapse) + cos(timeElapse) })));
    } else if(_renderer->getPipeline() == _effectPipeline)
        _effectVShader.setEffect(EFFECT_SIZE - (EFFECT_SIZE / timeElapse), ((unsigned)floor(timeElapse / 5.0) % EFFECT_ITER) + 3);
    else if(_renderer->getPipeline() == _beamsPipeline){
        _beamsVShader.setLight(LIGHT_Sky, Topl_Light(Vec3f({ sin((*(camera.getRot())).data[0]), cos((*(camera.getRot())).data[0]), tan((*(camera.getRot())).data[1]) }), { BEAMS_SKY_LIGHT.value }));
        _beamsVShader.setLight(LIGHT_Flash, Topl_Light(*camera.getPos(), { BEAMS_FLASH_LIGHT.value }));
        if(Platform::mouseControl.getTracerSteps()->size() > 0) _beamsVShader.setLight(LIGHT_Lamp, Topl_Light(Vec3f({ (float)tracerStep.step.first, (float)tracerStep.step.second, 0.0F }), { BEAMS_LAMP_LIGHT.value }));
    } else if(_renderer->getPipeline() == _fieldPipeline || _renderer->getPipeline() == _patternPipeline)
        for(unsigned p = 0; p < FIELD_POINTS_MAX && p < Platform::mouseControl.getTracerSteps()->size(); p++){
            tracerStep = (*Platform::mouseControl.getTracerSteps())[Platform::mouseControl.getTracerSteps()->size() - p - 1];
            if(_renderer->getPipeline() == _fieldPipeline) _fieldVShader.setCtrlPoint(p, Vec3f({
              (float)tracerStep.step.first + (sin((float)tracerStep.step.first + timeElapse / 100.0F) * 0.05F),
              (float)tracerStep.step.second + (sin((float)tracerStep.step.second + timeElapse / 100.0F) * 0.05F),
              DEFAULT_Z
            }));
        }
}
