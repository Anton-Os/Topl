#ifndef SKY_SHADER_H
#define SKY_SHADER_H

#include "modern/Modern_Shader.hpp"

// Fullscreen sky with clouds, stars and aurora driven by the FPS camera rays

// Vertex Shaders

struct Sky_VertexShader : public Modern_VertexShader {
	Sky_VertexShader() : Modern_VertexShader(){ }
	Sky_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Sky_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }
};

struct Sky_VertexShader_GL4 : public Sky_VertexShader {
	Sky_VertexShader_GL4() : Sky_VertexShader("modern/sky/glsl/Sky_Vertex.glsl") {}
	Sky_VertexShader_GL4(int mode) : Sky_VertexShader("modern/sky/glsl/Sky_Vertex.glsl", mode) {}
};

struct Sky_VertexShader_DX11 : public Sky_VertexShader {
	Sky_VertexShader_DX11() : Sky_VertexShader("modern/sky/hlsl/Sky_Vertex.hlsl") {}
	Sky_VertexShader_DX11(int mode) : Sky_VertexShader("modern/sky/hlsl/Sky_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Sky_PixelShader : public Modern_PixelShader {
	Sky_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Sky_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Sky_PixelShader_GL4 : public Sky_PixelShader {
	Sky_PixelShader_GL4() : Sky_PixelShader("modern/sky/glsl/Sky_Frag.glsl") {}
};

struct Sky_PixelShader_DX11 : public Sky_PixelShader {
	Sky_PixelShader_DX11() : Sky_PixelShader("modern/sky/hlsl/Sky_Pixel.hlsl") {}
};

#endif
