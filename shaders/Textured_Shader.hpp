#include "Topl_Pipeline.hpp"

#define TEX_BASE 0 // default base texture
#define TEX_1 1
#define TEX_2 2
#define TEX_3 3
#define TEX_4 4
#define TEX_5 5
#define TEX_6 6
#define TEX_7 7
#define TEX_VOLUME 8 // volumetric texture
#define TEX_SLICE 9 // volumetric slice

// TODO: Include more modes

// Vertex Shaders

struct Textured_VertexShader : public Topl_EntryShader {
	Textured_VertexShader() : Topl_EntryShader(){}
	Textured_VertexShader(std::string name) : Topl_EntryShader(name) {  }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Topl_EntryShader::genActorBlock(actor, bytes);
		auto paramEntry = std::find_if(texParamMap.begin(), texParamMap.end(), [actor](const std::pair<actor_cptr, TexParams> p){ return p.first == actor; });
		auto antialiasEntry = std::find_if(antialiasMap.begin(), antialiasMap.end(), [actor](const std::pair<actor_cptr, std::pair<float, unsigned>> p){ return p.first == actor; });
		appendDataToBytes((paramEntry != texParamMap.end())? (uint8_t*)&paramEntry->second.scroll : (uint8_t*)&_texScroll, sizeof(Vec3f), bytes);
		appendDataToBytes((paramEntry != texParamMap.end())? (uint8_t*)&paramEntry->second.scale : (uint8_t*)&_texScale, sizeof(Vec3f), bytes);
        alignDataToBytes((paramEntry != texParamMap.end())? (uint8_t*)&paramEntry->second.flip : (uint8_t*)&_flip, sizeof(unsigned), NO_PADDING, bytes);
		alignDataToBytes((antialiasEntry != antialiasMap.end())? (uint8_t*)&antialiasEntry->second.first : (uint8_t*)&_antialiasArea, sizeof(unsigned), NO_PADDING, bytes);
		alignDataToBytes((antialiasEntry != antialiasMap.end())? (uint8_t*)&antialiasEntry->second.second : (uint8_t*)&_antialiasSteps, sizeof(float), NO_PADDING, bytes);
        alignDataToBytes((paramEntry != texParamMap.end())? (uint8_t*)&paramEntry->second.slice : (uint8_t*)&_slice, sizeof(float), NO_PADDING, bytes);
	}

	void setFlip(unsigned f){ _flip = f; }
	void setSlice(float s){ _slice = s; }
	void setTexScroll(const Vec3f& s){ _texScroll = s; }
	void setTexScale(const Vec3f& s){ _texScale = s; }

	struct TexParams { 
		unsigned flip;
		float slice;
		Vec3f scroll;
		Vec3f scale;
	};

	void setParams(actor_cptr actor, TexParams params){
		auto paramEntry = std::find_if(texParamMap.begin(), texParamMap.end(), [actor](const std::pair<actor_cptr, TexParams> p){ return p.first == actor; });
		if(paramEntry != texParamMap.end()) texParamMap[actor] = params; 
		else texParamMap.insert({ actor, params });
	}

	void setAntialiasing(float area, unsigned steps){
		_antialiasArea = area;
		_antialiasSteps = steps;
	}

	void setAntialiasing(actor_cptr actor, float area, unsigned steps){
		auto antialiasEntry = std::find_if(antialiasMap.begin(), antialiasMap.end(), [actor](const std::pair<actor_cptr, std::pair<float, unsigned>> p){ return p.first == actor; });
		if(antialiasEntry != antialiasMap.end()) antialiasMap[actor] = std::make_pair(area, steps); 
		else antialiasMap.insert({ actor, std::make_pair(area, steps) });
	}
private:
	Vec3f _texScroll = Vec3f({ 0.0, 0.0, 0.0 });
	Vec3f _texScale = Vec3f({ 1.0, 1.0, 1.0 });

    float _slice = 0.0; // slice to use for 3D texture
    unsigned _flip = 0; // orientation for 2D texture

    float _antialiasArea = 0.0F; // area for antialiasing
    unsigned _antialiasSteps = 0; // iterations for antialiasing

	std::map<actor_cptr, TexParams> texParamMap;
	std::map<actor_cptr, std::pair<float, unsigned>> antialiasMap;
};

struct Textured_VertexShader_GL4 : public Textured_VertexShader {
	Textured_VertexShader_GL4() : Textured_VertexShader(genPrefix_glsl() + "vertex/" + "Textured_Vertex.glsl") {}
};

struct Textured_VertexShader_DX11 : public Textured_VertexShader {
	Textured_VertexShader_DX11() : Textured_VertexShader(genPrefix_hlsl() + "vertex/" + "Textured_Vertex.hlsl") {}
};

// Pixel Shaders

struct Textured_PixelShader : public Topl_Shader {
	Textured_PixelShader() : Topl_Shader(){}
	Textured_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name) { }
};

struct Textured_PixelShader_GL4 : public Textured_PixelShader {
	Textured_PixelShader_GL4() : Textured_PixelShader(genPrefix_glsl() + "pixel/" + "Textured_Frag.glsl") {}
};

struct Textured_PixelShader_DX11 : public Textured_PixelShader {
	Textured_PixelShader_DX11() : Textured_PixelShader(genPrefix_hlsl() + "pixel/" + "Textured_Pixel.hlsl") { }
};