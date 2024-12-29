#include "Platform.hpp"

#include "Topl_Pipeline.hpp"

#define CANVAS_MODE_DOT 0

#define CANVAS_STEPS 16
#define CANVAS_PATHS 16

// Vertex Shaders

struct Canvas_VertexShader : public Topl_EntryShader {
	Canvas_VertexShader() : Topl_EntryShader(){}
	Canvas_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Canvas_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		Vec2i screenRes = Vec2i({ width, height });
		Vec2f cursorPos = Vec2f({ Platform::getCursorX(), Platform::getCursorY() });
	
		Topl_EntryShader::genSceneBlock(scene, bytes);
		alignDataToBytes((uint8_t*)&screenRes.data[0], sizeof(screenRes), NO_PADDING, bytes);
		alignDataToBytes((uint8_t*)&cursorPos.data[0], sizeof(cursorPos), NO_PADDING, bytes);
		// alignDataToBytes((uint8_t*)&drawSize, sizeof(drawSize), NO_PADDING, bytes);

		sendTracerData(bytes);
	}

	void setWidth(int w) { if(w > 0) width = w; }
	void setHeight(int h) { if(h > 0) height = h; }
protected:
	void sendTracerData(blockBytes_t* bytes) const {
		static Vec2f steps[CANVAS_STEPS];
		for(unsigned short t = 0; t < CANVAS_STEPS; t++)
			if(t < Platform::mouseControl.getTracerSteps()->size()){
				Input_TracerStep tracerStep = (*Platform::mouseControl.getTracerSteps())[Platform::mouseControl.getTracerSteps()->size() - t - 1];
				steps[t] = Vec2f({ tracerStep.step.first, tracerStep.step.second });
			}
			else steps[t] = VEC_2F_ZERO; // Vec2f({ INVALID_CURSOR_POS, INVALID_CURSOR_POS });
		alignDataToBytes((uint8_t*)&steps[0], sizeof(Vec2f) * 8, NO_PADDING, bytes);

		static Vec2f paths[CANVAS_PATHS];
		for(unsigned short t = 0; t < CANVAS_PATHS; t++) paths[t] = VEC_2F_ZERO; // Vec2f({ INVALID_CURSOR_POS, INVALID_CURSOR_POS });
		if(Platform::mouseControl.getTracerPaths()->size() > 0){
			Input_TracerPath tracerPath = Platform::mouseControl.getTracerPaths()->back();
			for(unsigned short t = 0; t < tracerPath.stepsCount && t < CANVAS_PATHS; t++)
				paths[t] = Vec2f({ tracerPath.steps[t].first, tracerPath.steps[t].second });
		}
		alignDataToBytes((uint8_t*)&paths[0], sizeof(Vec2f) * 8, NO_PADDING, bytes); 
	}

	int width = TOPL_WIN_WIDTH;
	int height = TOPL_WIN_HEIGHT;
	double drawSize = 0.05;
};

struct Canvas_VertexShader_GL4 : public Canvas_VertexShader {
    Canvas_VertexShader_GL4() : Canvas_VertexShader(genPrefix_glsl() + "Canvas_Vertex.glsl"){}
	Canvas_VertexShader_GL4(unsigned mode) : Canvas_VertexShader(genPrefix_glsl() + "Canvas_Vertex.glsl", mode){}
};

struct Canvas_VertexShader_DX11 : public Canvas_VertexShader {
    Canvas_VertexShader_DX11() : Canvas_VertexShader(genPrefix_hlsl() + "Canvas_Vertex.hlsl"){}
	Canvas_VertexShader_DX11(unsigned mode) : Canvas_VertexShader(genPrefix_hlsl() + "Canvas_Vertex.hlsl", mode){}
};

// Pixel Shaders

struct Canvas_PixelShader : public Topl_Shader {
	Canvas_PixelShader() : Topl_Shader(){}
	Canvas_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Canvas_PixelShader_GL4 : public Canvas_PixelShader {
	Canvas_PixelShader_GL4() : Canvas_PixelShader(genPrefix_glsl() + "Canvas_Frag.glsl") { }
};

struct Canvas_PixelShader_DX11 : public Canvas_PixelShader {
	Canvas_PixelShader_DX11() : Canvas_PixelShader(genPrefix_hlsl() + "Canvas_Pixel.hlsl") { }
};
