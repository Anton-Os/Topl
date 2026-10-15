#ifndef SPECTRUM_SHADER_H
#define SPECTRUM_SHADER_H

#include "modern/Modern_Shader.hpp"

// Audio reactive displacement and neon coloring from spectrum bands

#define SPECTRUM_BANDS 16 // matches Spectrum.glsl and Spectrum.hlsl

// Vertex Shaders

struct Spectrum_VertexShader : public Modern_VertexShader {
	Spectrum_VertexShader() : Modern_VertexShader(){ }
	Spectrum_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Spectrum_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	void setBands(const float* levels, unsigned count){ // up to SPECTRUM_BANDS values in range 0 to 1
		for(unsigned b = 0; b < SPECTRUM_BANDS; b++) _params[b / 4][b % 4] = (b < count)? levels[b] : 0.0F;
	}
	void setSummary(float bass, float mids, float treble, float level){ _params[SPECTRUM_BANDS / 4] = Modern_Float4(bass, mids, treble, level); }
	float getBand(unsigned index) const { return _params[(index % SPECTRUM_BANDS) / 4][index % 4]; }
};

struct Spectrum_VertexShader_GL4 : public Spectrum_VertexShader {
	Spectrum_VertexShader_GL4() : Spectrum_VertexShader("modern/spectrum/glsl/Spectrum_Vertex.glsl") {}
	Spectrum_VertexShader_GL4(int mode) : Spectrum_VertexShader("modern/spectrum/glsl/Spectrum_Vertex.glsl", mode) {}
};

struct Spectrum_VertexShader_DX11 : public Spectrum_VertexShader {
	Spectrum_VertexShader_DX11() : Spectrum_VertexShader("modern/spectrum/hlsl/Spectrum_Vertex.hlsl") {}
	Spectrum_VertexShader_DX11(int mode) : Spectrum_VertexShader("modern/spectrum/hlsl/Spectrum_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Spectrum_PixelShader : public Modern_PixelShader {
	Spectrum_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Spectrum_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Spectrum_PixelShader_GL4 : public Spectrum_PixelShader {
	Spectrum_PixelShader_GL4() : Spectrum_PixelShader("modern/spectrum/glsl/Spectrum_Frag.glsl") {}
};

struct Spectrum_PixelShader_DX11 : public Spectrum_PixelShader {
	Spectrum_PixelShader_DX11() : Spectrum_PixelShader("modern/spectrum/hlsl/Spectrum_Pixel.hlsl") {}
};

#endif
