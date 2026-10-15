#ifndef LIGHTING_SHADER_H
#define LIGHTING_SHADER_H

#include "modern/Modern_Shader.hpp"

// Blinn-Phong lighting with fresnel sky reflections and fog

// Vertex Shaders

struct Lighting_VertexShader : public Modern_VertexShader {
	Lighting_VertexShader() : Modern_VertexShader(){ }
	Lighting_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Lighting_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }
};

struct Lighting_VertexShader_GL4 : public Lighting_VertexShader {
	Lighting_VertexShader_GL4() : Lighting_VertexShader("modern/lighting/glsl/Lighting_Vertex.glsl") {}
	Lighting_VertexShader_GL4(int mode) : Lighting_VertexShader("modern/lighting/glsl/Lighting_Vertex.glsl", mode) {}
};

struct Lighting_VertexShader_DX11 : public Lighting_VertexShader {
	Lighting_VertexShader_DX11() : Lighting_VertexShader("modern/lighting/hlsl/Lighting_Vertex.hlsl") {}
	Lighting_VertexShader_DX11(int mode) : Lighting_VertexShader("modern/lighting/hlsl/Lighting_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Lighting_PixelShader : public Modern_PixelShader {
	Lighting_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Lighting_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Lighting_PixelShader_GL4 : public Lighting_PixelShader {
	Lighting_PixelShader_GL4() : Lighting_PixelShader("modern/lighting/glsl/Lighting_Frag.glsl") {}
};

struct Lighting_PixelShader_DX11 : public Lighting_PixelShader {
	Lighting_PixelShader_DX11() : Lighting_PixelShader("modern/lighting/hlsl/Lighting_Pixel.hlsl") {}
};

#endif
