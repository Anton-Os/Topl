#ifndef TOON_SHADER_H
#define TOON_SHADER_H

#include "modern/Modern_Shader.hpp"

// Cel shading with rim light, ink outlines, halftone and hatching

// Vertex Shaders

struct Toon_VertexShader : public Modern_VertexShader {
	Toon_VertexShader() : Modern_VertexShader(){ }
	Toon_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Toon_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }
};

struct Toon_VertexShader_GL4 : public Toon_VertexShader {
	Toon_VertexShader_GL4() : Toon_VertexShader("modern/toon/glsl/Toon_Vertex.glsl") {}
	Toon_VertexShader_GL4(int mode) : Toon_VertexShader("modern/toon/glsl/Toon_Vertex.glsl", mode) {}
};

struct Toon_VertexShader_DX11 : public Toon_VertexShader {
	Toon_VertexShader_DX11() : Toon_VertexShader("modern/toon/hlsl/Toon_Vertex.hlsl") {}
	Toon_VertexShader_DX11(int mode) : Toon_VertexShader("modern/toon/hlsl/Toon_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Toon_PixelShader : public Modern_PixelShader {
	Toon_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Toon_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Toon_PixelShader_GL4 : public Toon_PixelShader {
	Toon_PixelShader_GL4() : Toon_PixelShader("modern/toon/glsl/Toon_Frag.glsl") {}
};

struct Toon_PixelShader_DX11 : public Toon_PixelShader {
	Toon_PixelShader_DX11() : Toon_PixelShader("modern/toon/hlsl/Toon_Pixel.hlsl") {}
};

#endif
