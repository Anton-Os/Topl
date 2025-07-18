#include "Topl_Pipeline.hpp"

#define FIELD_POINTS_MAX 16

// Vertex Shaders

struct Field_VertexShader : public Topl_EntryShader {
	Field_VertexShader() : Topl_EntryShader(){ }
	Field_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Field_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

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
		for(auto p = points.begin(); p != points.end() && idx < FIELD_POINTS_MAX; p++){
			ctrlPoints[idx] = *p;
			idx++;
		}
	}

	void setCtrlPoint(unsigned short index, Vec3f point){ ctrlPoints[index % FIELD_POINTS_MAX] = point; }

	bool isAutoCtrl = true;
protected:
	Mat4x4 ctrlMatrix = MAT_4x4_IDENTITY;
	std::map<actor_cptr, Mat4x4> ctrlMatrixMap;
	std::map<actor_cptr, float> alphaValMap;

	Vec3f ctrlPoints[FIELD_POINTS_MAX] = {
		{ 0.0F, 0.0F, 0.0F }, { 0.25F, 0.25F, 0.0F }, { -0.25F, -0.25F, 0.0F }, { -0.5F, 0.5F, 0.0F },  
		{ 0.5F, -0.5F, 0.0F }, { 0.75F, 0.75F, 0.0F }, { -0.75F, -0.75F, 0.0F }, { 1.0F, 1.0F, 0.0F },
	};
};

struct Field_VertexShader_GL4 : public Field_VertexShader {
	Field_VertexShader_GL4() : Field_VertexShader(genPrefix_glsl() + "vertex/" + "Field_Vertex.glsl") {}
	Field_VertexShader_GL4(unsigned mode) : Field_VertexShader(genPrefix_glsl() + "vertex/" + "Field_Vertex.glsl", mode) {}
};

struct Field_VertexShader_DX11 : public Field_VertexShader {
	Field_VertexShader_DX11() : Field_VertexShader(genPrefix_hlsl() + "vertex/" + "Field_Vertex.hlsl") {}
	Field_VertexShader_DX11(unsigned mode) : Field_VertexShader(genPrefix_hlsl() + "vertex/" + "Field_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Field_PixelShader : public Topl_Shader {
	Field_PixelShader() : Topl_Shader(){} // Blank Constructor
	Field_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Field_PixelShader_GL4 : public Field_PixelShader {
	Field_PixelShader_GL4() : Field_PixelShader(genPrefix_glsl() + "pixel/" + "Field_Frag.glsl") {}
};

struct Field_PixelShader_DX11 : public Field_PixelShader {
	Field_PixelShader_DX11() : Field_PixelShader(genPrefix_hlsl() + "pixel/" + "Field_Pixel.hlsl") {}
};
