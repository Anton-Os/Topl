#include "Topl_Pipeline.hpp"

#define DYNAMIC_MODE_RAINBOW 0

#define DYNAMIC_POINTS_MAX 8

// Vertex Shaders

struct Experimental_VertexShader : public Topl_EntryShader {
	Experimental_VertexShader() : Topl_EntryShader(){ }
	Experimental_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Experimental_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const {
		Mat4x4 idMatrix = MAT_4x4_IDENTITY;

		Topl_EntryShader::genActorBlock(actor, bytes);
		if(actor == targetActor) appendDataToBytes((uint8_t*)&ctrlMatrix, sizeof(ctrlMatrix), bytes);
		else appendDataToBytes((uint8_t*)&idMatrix, sizeof(idMatrix), bytes);
	}

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		static Timer_Dynamic dynamic_timer = Timer_Dynamic(0.0);
        double relMillisecs = dynamic_timer.getRelMillisecs();
        double absMillisecs = dynamic_timer.getAbsMillisecs();

		Topl_EntryShader::genSceneBlock(scene, bytes);
        alignDataToBytes((uint8_t*)&relMillisecs, sizeof(relMillisecs), NO_PADDING, bytes);
        alignDataToBytes((uint8_t*)&absMillisecs, sizeof(absMillisecs), NO_PADDING, bytes);
		alignDataToBytes((uint8_t*)&ctrlPoints, sizeof(ctrlPoints), NO_PADDING, bytes);
		// appendDataToBytes((uint8_t*)&lightPos, sizeof(Vec3f), bytes);
		// appendDataToBytes((uint8_t*)&lightVal, sizeof(Vec3f), bytes);
	}

	void setCtrlMatrix(const Geo_Actor* actor, Mat4x4 matrix){
		targetActor = actor;
		ctrlMatrix = matrix;
	}

	void setCtrlPoint(unsigned short index, Vec3f point){ ctrlPoints[index % DYNAMIC_POINTS_MAX] = point; }

	void setCtrlPoints(std::initializer_list<Vec3f> points){
		unsigned short idx = 0;
		for(auto p = points.begin(); p != points.end() && idx < DYNAMIC_POINTS_MAX; p++){
			ctrlPoints[idx] = *p;
			idx++;
		}
	}
protected:
	const Geo_Actor* targetActor = nullptr;
	Mat4x4 ctrlMatrix = MAT_4x4_IDENTITY;
	
	Vec3f ctrlPoints[DYNAMIC_POINTS_MAX] = {
		{ 0.0F, 0.0F, 0.0F }, { 0.25F, 0.25F, 0.0F }, { -0.25F, -0.25F, 0.0F }, { -0.5F, 0.5F, 0.0F },  
		{ 0.5F, -0.5F, 0.0F }, { 0.75F, 0.75F, 0.0F }, { -0.75F, -0.75F, 0.0F }, { 1.0F, 1.0F, 0.0F },
	};

	// Vec3f lightPos, lightVal;
};

struct Experimental_VertexShader_GL4 : public Experimental_VertexShader {
	Experimental_VertexShader_GL4() : Experimental_VertexShader(genPrefix_glsl() + "Experimental_Vertex.glsl") {}
	Experimental_VertexShader_GL4(unsigned mode) : Experimental_VertexShader(genPrefix_glsl() + "Experimental_Vertex.glsl", mode) {}
};

struct Experimental_VertexShader_DX11 : public Experimental_VertexShader {
	Experimental_VertexShader_DX11() : Experimental_VertexShader(genPrefix_hlsl() + "Experimental_Vertex.hlsl") {}
	Experimental_VertexShader_DX11(unsigned mode) : Experimental_VertexShader(genPrefix_hlsl() + "Experimental_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Experimental_PixelShader : public Topl_Shader {
	Experimental_PixelShader() : Topl_Shader(){} // Blank Constructor
	Experimental_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Experimental_PixelShader_GL4 : public Experimental_PixelShader {
	Experimental_PixelShader_GL4() : Experimental_PixelShader(genPrefix_glsl() + "Experimental_Frag.glsl") {}
};

struct Experimental_PixelShader_DX11 : public Experimental_PixelShader {
	Experimental_PixelShader_DX11() : Experimental_PixelShader(genPrefix_hlsl() + "Experimental_Pixel.hlsl") {}
};
