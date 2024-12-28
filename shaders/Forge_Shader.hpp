#include "Topl_Pipeline.hpp"

#define CONSTRUCT_VORNOI 0

#define CONSTRUCT_POINTS_MAX 8

// Vertex Shaders

struct Forge_VertexShader : public Topl_EntryShader {
	Forge_VertexShader() : Topl_EntryShader(){ }
	Forge_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Forge_VertexShader(std::string name, unsigned mode) : Topl_EntryShader(name) { _mode = mode; }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const {
		// Mat4x4 idMatrix = MAT_4x4_IDENTITY;
		Topl_EntryShader::genActorBlock(actor, bytes);

		// if(actor == targetActor) 
		appendDataToBytes((uint8_t*)&ctrlMatrix, sizeof(ctrlMatrix), bytes);
		// else appendDataToBytes((uint8_t*)&idMatrix, sizeof(idMatrix), bytes);
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

	void setCtrlMatrix(Mat4x4 matrix){ ctrlMatrix = matrix; }

	void setCtrlPoint(unsigned short index, Vec3f point){ ctrlPoints[index % CONSTRUCT_POINTS_MAX] = point; }

	void setCtrlPoints(std::initializer_list<Vec3f> points){
		unsigned short idx = 0;
		for(auto p = points.begin(); p != points.end() && idx < CONSTRUCT_POINTS_MAX; p++){
			ctrlPoints[idx] = *p;
			idx++;
		}
	}
protected:
	// const Geo_Actor* targetActor = nullptr;
	Mat4x4 ctrlMatrix = MAT_4x4_IDENTITY;
	
	Vec3f ctrlPoints[CONSTRUCT_POINTS_MAX] = {
		{ 0.0F, 0.0F, 0.0F }, { 0.25F, 0.25F, 0.0F }, { -0.25F, -0.25F, 0.0F }, { -0.5F, 0.5F, 0.0F },  
		{ 0.5F, -0.5F, 0.0F }, { 0.75F, 0.75F, 0.0F }, { -0.75F, -0.75F, 0.0F }, { 1.0F, 1.0F, 0.0F },
	};

	// Vec3f lightPos, lightVal;
};

struct Forge_VertexShader_GL4 : public Forge_VertexShader {
	Forge_VertexShader_GL4() : Forge_VertexShader(genPrefix_glsl() + "Forge_Vertex.glsl") {}
	Forge_VertexShader_GL4(unsigned mode) : Forge_VertexShader(genPrefix_glsl() + "Forge_Vertex.glsl", mode) {}
};

struct Forge_VertexShader_DX11 : public Forge_VertexShader {
	Forge_VertexShader_DX11() : Forge_VertexShader(genPrefix_hlsl() + "Forge_Vertex.hlsl") {}
	Forge_VertexShader_DX11(unsigned mode) : Forge_VertexShader(genPrefix_hlsl() + "Forge_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Forge_PixelShader : public Topl_Shader {
	Forge_PixelShader() : Topl_Shader(){} // Blank Constructor
	Forge_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

struct Forge_PixelShader_GL4 : public Forge_PixelShader {
	Forge_PixelShader_GL4() : Forge_PixelShader(genPrefix_glsl() + "Forge_Frag.glsl") {}
};

struct Forge_PixelShader_DX11 : public Forge_PixelShader {
	Forge_PixelShader_DX11() : Forge_PixelShader(genPrefix_hlsl() + "Forge_Pixel.hlsl") {}
};
