#ifndef RAYMARCH_SHADER_H
#define RAYMARCH_SHADER_H

#include "modern/Modern_Shader.hpp"

// Signed distance field raymarcher with soft shadows and occlusion

// Vertex Shaders

struct Raymarch_VertexShader : public Modern_VertexShader {
	Raymarch_VertexShader() : Modern_VertexShader(){ }
	Raymarch_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Raymarch_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }
};

struct Raymarch_VertexShader_GL4 : public Raymarch_VertexShader {
	Raymarch_VertexShader_GL4() : Raymarch_VertexShader("modern/raymarch/glsl/Raymarch_Vertex.glsl") {}
	Raymarch_VertexShader_GL4(int mode) : Raymarch_VertexShader("modern/raymarch/glsl/Raymarch_Vertex.glsl", mode) {}
};

struct Raymarch_VertexShader_DX11 : public Raymarch_VertexShader {
	Raymarch_VertexShader_DX11() : Raymarch_VertexShader("modern/raymarch/hlsl/Raymarch_Vertex.hlsl") {}
	Raymarch_VertexShader_DX11(int mode) : Raymarch_VertexShader("modern/raymarch/hlsl/Raymarch_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Raymarch_PixelShader : public Modern_PixelShader {
	Raymarch_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Raymarch_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Raymarch_PixelShader_GL4 : public Raymarch_PixelShader {
	Raymarch_PixelShader_GL4() : Raymarch_PixelShader("modern/raymarch/glsl/Raymarch_Frag.glsl") {}
};

struct Raymarch_PixelShader_DX11 : public Raymarch_PixelShader {
	Raymarch_PixelShader_DX11() : Raymarch_PixelShader("modern/raymarch/hlsl/Raymarch_Pixel.hlsl") {}
};

#endif
