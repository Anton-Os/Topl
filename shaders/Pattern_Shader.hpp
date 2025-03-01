#include "Topl_Pipeline.hpp"

#define PATTERN_POINTS_MAX 8

// Vertex Shaders

struct Pattern_VertexShader : public Topl_EntryShader {
	Pattern_VertexShader() : Topl_EntryShader(){ }
	Pattern_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Pattern_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(actor_cptr actor, blockBytes_t* bytes) const {
		static float alpha;
		static Mat4x4 matrix = (isAutoCtrl)? Mat4x4::translation(*(actor->getPos())) : ctrlMatrix;

		Topl_EntryShader::genActorBlock(actor, bytes);

		auto matrixEntry = std::find_if(ctrlMatrixMap.begin(), ctrlMatrixMap.end(), [actor](const std::pair<actor_cptr, Mat4x4> p){ return p.first == actor; });
		auto alphaEntry = std::find_if(alphaValMap.begin(), alphaValMap.end(), [actor](const std::pair<actor_cptr, float> a){ return a.first == actor; });
		alpha = (alphaEntry != alphaValMap.end())? alphaEntry->second : 1.0F;

		appendDataToBytes((matrixEntry != ctrlMatrixMap.end())? (uint8_t*)&matrixEntry->second : (uint8_t*)&matrix, sizeof(Mat4x4), bytes);
		appendDataToBytes((uint8_t*)&alpha, sizeof(float), bytes);
	}

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		static Timer_Dynamic dynamic_timer = Timer_Dynamic(0.0);
		
        double relMillisecs = dynamic_timer.getRelMillisecs();
        double absMillisecs = dynamic_timer.getAbsMillisecs();

		Topl_EntryShader::genSceneBlock(scene, bytes);
        alignDataToBytes((uint8_t*)&relMillisecs, sizeof(relMillisecs), NO_PADDING, bytes);
        alignDataToBytes((uint8_t*)&absMillisecs, sizeof(absMillisecs), NO_PADDING, bytes);
		alignDataToBytes((uint8_t*)&ctrlPoints, sizeof(ctrlPoints), NO_PADDING, bytes);
	}

	void setCtrlMatrix(actor_cptr actor, Mat4x4 matrix){ ctrlMatrixMap.insert({ actor, matrix }); }

	void setAlpha(actor_cptr actor, float a){ alphaValMap.insert({ actor, a }); }

	void setCtrlMatrix(Mat4x4 matrix){ ctrlMatrix = matrix; }

	void setCtrlPoints(std::initializer_list<Vec3f> points){
		unsigned short idx = 0;
		for(auto p = points.begin(); p != points.end() && idx < PATTERN_POINTS_MAX; p++){
			ctrlPoints[idx] = *p;
			idx++;
		}
	}

	void setCtrlPoint(unsigned short index, Vec3f point){ ctrlPoints[index % PATTERN_POINTS_MAX] = point; }

	bool isAutoCtrl = true;
protected:
	Mat4x4 ctrlMatrix = MAT_4x4_IDENTITY;
	std::map<actor_cptr, Mat4x4> ctrlMatrixMap;
	std::map<actor_cptr, float> alphaValMap;

	Vec3f ctrlPoints[PATTERN_POINTS_MAX] = {
		{ 0.0F, 0.0F, 0.0F }, { 0.25F, 0.25F, 0.0F }, { -0.25F, -0.25F, 0.0F }, { -0.5F, 0.5F, 0.0F },  
		{ 0.5F, -0.5F, 0.0F }, { 0.75F, 0.75F, 0.0F }, { -0.75F, -0.75F, 0.0F }, { 1.0F, 1.0F, 0.0F },
	};
};

struct Pattern_VertexShader_GL4 : public Pattern_VertexShader {
	Pattern_VertexShader_GL4() : Pattern_VertexShader(genPrefix_glsl() + "vertex/" + "Pattern_Vertex.glsl") {}
	Pattern_VertexShader_GL4(unsigned mode) : Pattern_VertexShader(genPrefix_glsl() + "vertex/" + "Pattern_Vertex.glsl", mode) {}
};

struct Pattern_VertexShader_DX11 : public Pattern_VertexShader {
	Pattern_VertexShader_DX11() : Pattern_VertexShader(genPrefix_hlsl() + "vertex/" + "Pattern_Vertex.hlsl") {}
	Pattern_VertexShader_DX11(unsigned mode) : Pattern_VertexShader(genPrefix_hlsl() + "vertex/" + "Pattern_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Pattern_PixelShader : public Topl_Shader {
	Pattern_PixelShader() : Topl_Shader(){} // Blank Constructor
	Pattern_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Pattern_PixelShader_GL4 : public Pattern_PixelShader {
	Pattern_PixelShader_GL4() : Pattern_PixelShader(genPrefix_glsl() + "pixel/" + "Pattern_Frag.glsl") {}
};

struct Pattern_PixelShader_DX11 : public Pattern_PixelShader {
	Pattern_PixelShader_DX11() : Pattern_PixelShader(genPrefix_hlsl() + "pixel/" + "Pattern_Pixel.hlsl") {}
};
