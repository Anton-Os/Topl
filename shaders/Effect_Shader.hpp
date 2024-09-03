#include "Platform.hpp"

#include "Topl_Pipeline.hpp"

#define EFFECT_MODE_FRACTALS 0
#define EFFECT_MODE_CURSORY 1

// Vertex Shaders

struct Effect_VertexShader : public Topl_EntryShader {
	Effect_VertexShader() : Topl_EntryShader(){}
	Effect_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Effect_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		Vec2i screenRes = Vec2i({ width, height });
		Vec2f cursorPos = Vec2f({ Platform::getCursorX(), Platform::getCursorY() });
	
		Topl_EntryShader::genSceneBlock(scene, camera, bytes);
		alignDataToBytes((uint8_t*)&screenRes.data[0], sizeof(screenRes), NO_PADDING, bytes);
		alignDataToBytes((uint8_t*)&cursorPos.data[0], sizeof(cursorPos), NO_PADDING, bytes);

		sendTracerData(bytes);
	}

	virtual void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Topl_EntryShader::genActorBlock(actor, bytes);
	}

	void setWidth(int w) { if(w > 0) width = w; }
	void setHeight(int h) { if(h > 0) height = h; }
protected:
	void sendTracerData(blockBytes_t* bytes) const {
		static Vec2f steps[8];
		for(unsigned short t = 0; t < 8; t++)
			if(t < Platform::mouseControl.getTracerSteps()->size()){
				Input_TracerStep tracerStep = (*Platform::mouseControl.getTracerSteps())[Platform::mouseControl.getTracerSteps()->size() - t - 1];
				steps[t] = Vec2f({ tracerStep.step.first, tracerStep.step.second });
			}
			else steps[t] = Vec2f({ INVALID_CURSOR_POS, INVALID_CURSOR_POS });
		alignDataToBytes((uint8_t*)&steps[0], sizeof(Vec2f) * 8, NO_PADDING, bytes);

		static Vec2f paths[MAX_PATH_STEPS];
		for(unsigned short t = 0; t < MAX_PATH_STEPS; t++) paths[t] = Vec2f({ INVALID_CURSOR_POS, INVALID_CURSOR_POS });
		if(Platform::mouseControl.getTracerPaths()->size() > 0){
			Input_TracerPath tracerPath = Platform::mouseControl.getTracerPaths()->back();
			for(unsigned short t = 0; t < tracerPath.stepsCount && t < MAX_PATH_STEPS; t++)
				paths[t] = Vec2f({ tracerPath.steps[t].first, tracerPath.steps[t].second });
		}
		alignDataToBytes((uint8_t*)&paths[0], sizeof(Vec2f) * 8, NO_PADDING, bytes); 
	}

	unsigned _mode = EFFECT_MODE_CURSORY;
	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;
};

struct Effect_VertexShader_GL4 : public Effect_VertexShader {
    Effect_VertexShader_GL4() : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl"){}
	Effect_VertexShader_GL4(unsigned mode) : Effect_VertexShader(genPrefix_glsl() + "Effect_Vertex.glsl", mode){}
};

struct Effect_VertexShader_DX11 : public Effect_VertexShader {
    Effect_VertexShader_DX11() : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl"){}
	Effect_VertexShader_DX11(unsigned mode) : Effect_VertexShader(genPrefix_hlsl() + "Effect_Vertex.hlsl", mode){}
};

// Pixel Shaders

struct Effect_PixelShader : public Topl_Shader {
	Effect_PixelShader() : Topl_Shader(){}
	Effect_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Effect_PixelShader_GL4 : public Effect_PixelShader {
	Effect_PixelShader_GL4() : Effect_PixelShader(genPrefix_glsl() + "Effect_Frag.glsl") { }
};

struct Effect_PixelShader_DX11 : public Effect_PixelShader {
	Effect_PixelShader_DX11() : Effect_PixelShader(genPrefix_hlsl() + "Effect_Pixel.hlsl") { }
};