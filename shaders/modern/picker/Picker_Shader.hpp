#ifndef PICKER_SHADER_H
#define PICKER_SHADER_H

#include "modern/Modern_Shader.hpp"

// Object picking through the FPS camera: each actor is drawn in a gray level equal to the low byte of its id
// so the pixel under the cursor identifies the actor regardless of the framebuffer channel order

#define PICKER_ACTOR_COLOR_OFFSET 48 // byte offset of actor_color inside the modern actor block

// Vertex Shaders

struct Picker_VertexShader : public Modern_VertexShader {
	Picker_VertexShader() : Modern_VertexShader(){ }
	Picker_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Picker_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	static unsigned getPickByte(const Geo_Actor* actor){ return (actor != nullptr)? (actor->getId() & 0xFF) : 0; }

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Modern_VertexShader::genActorBlock(actor, bytes);
		float level = getPickByte(actor) / 255.0F;
		float idColor[4] = { level, level, level, 1.0F };
		if(bytes->size() >= PICKER_ACTOR_COLOR_OFFSET + sizeof(idColor))
			memcpy(bytes->data() + PICKER_ACTOR_COLOR_OFFSET, idColor, sizeof(idColor));
	}
};

struct Picker_VertexShader_GL4 : public Picker_VertexShader {
	Picker_VertexShader_GL4() : Picker_VertexShader("modern/picker/glsl/Picker_Vertex.glsl") {}
	Picker_VertexShader_GL4(int mode) : Picker_VertexShader("modern/picker/glsl/Picker_Vertex.glsl", mode) {}
};

struct Picker_VertexShader_DX11 : public Picker_VertexShader {
	Picker_VertexShader_DX11() : Picker_VertexShader("modern/picker/hlsl/Picker_Vertex.hlsl") {}
	Picker_VertexShader_DX11(int mode) : Picker_VertexShader("modern/picker/hlsl/Picker_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Picker_PixelShader : public Modern_PixelShader {
	Picker_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Picker_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Picker_PixelShader_GL4 : public Picker_PixelShader {
	Picker_PixelShader_GL4() : Picker_PixelShader("modern/picker/glsl/Picker_Frag.glsl") {}
};

struct Picker_PixelShader_DX11 : public Picker_PixelShader {
	Picker_PixelShader_DX11() : Picker_PixelShader("modern/picker/hlsl/Picker_Pixel.hlsl") {}
};

#endif
