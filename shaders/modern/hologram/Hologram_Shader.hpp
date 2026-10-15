#ifndef HOLOGRAM_SHADER_H
#define HOLOGRAM_SHADER_H

#include "modern/Modern_Shader.hpp"

// Glitching hologram with scanlines, fresnel rim and flicker

// Vertex Shaders

struct Hologram_VertexShader : public Modern_VertexShader {
	Hologram_VertexShader() : Modern_VertexShader(){ }
	Hologram_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Hologram_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }
};

struct Hologram_VertexShader_GL4 : public Hologram_VertexShader {
	Hologram_VertexShader_GL4() : Hologram_VertexShader("modern/hologram/glsl/Hologram_Vertex.glsl") {}
	Hologram_VertexShader_GL4(int mode) : Hologram_VertexShader("modern/hologram/glsl/Hologram_Vertex.glsl", mode) {}
};

struct Hologram_VertexShader_DX11 : public Hologram_VertexShader {
	Hologram_VertexShader_DX11() : Hologram_VertexShader("modern/hologram/hlsl/Hologram_Vertex.hlsl") {}
	Hologram_VertexShader_DX11(int mode) : Hologram_VertexShader("modern/hologram/hlsl/Hologram_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Hologram_PixelShader : public Modern_PixelShader {
	Hologram_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Hologram_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Hologram_PixelShader_GL4 : public Hologram_PixelShader {
	Hologram_PixelShader_GL4() : Hologram_PixelShader("modern/hologram/glsl/Hologram_Frag.glsl") {}
};

struct Hologram_PixelShader_DX11 : public Hologram_PixelShader {
	Hologram_PixelShader_DX11() : Hologram_PixelShader("modern/hologram/hlsl/Hologram_Pixel.hlsl") {}
};

#endif
