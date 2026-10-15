#ifndef SPACETIME_SHADER_H
#define SPACETIME_SHADER_H

#include "modern/Modern_Shader.hpp"

// Curved spacetime fabric pulled down by orbiting masses

#define SPACETIME_MASSES 4 // matches Spacetime.glsl and Spacetime.hlsl

// Vertex Shaders

struct Spacetime_VertexShader : public Modern_VertexShader {
	Spacetime_VertexShader() : Modern_VertexShader(){ }
	Spacetime_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Spacetime_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	void setMass(unsigned index, Vec3f pos, float mass){ if(index < SPACETIME_MASSES) _params[index] = Modern_Float4(pos[0], pos[1], pos[2], mass); }
	void setField(float gravity, float softening, float waveAmp, float waveFreq){ _params[SPACETIME_MASSES] = Modern_Float4(gravity, softening, waveAmp, waveFreq); }
	Modern_Float4 getField() const { return _params[SPACETIME_MASSES]; }

	// CPU mirror of getWell() inside Spacetime.glsl and Spacetime.hlsl
	float getWell(float x, float z) const {
		float gravity = (_params[SPACETIME_MASSES][0] > 0.0F)? _params[SPACETIME_MASSES][0] : 1.0F;
		float soft = (_params[SPACETIME_MASSES][1] > 0.0F)? _params[SPACETIME_MASSES][1] : 1.0F;
		float depth = 0.0F;
		for(unsigned m = 0; m < SPACETIME_MASSES; m++){
			float dx = x - _params[m][0], dz = z - _params[m][2];
			depth += gravity * _params[m][3] / sqrt(dx * dx + dz * dz + soft * soft);
		}
		return depth;
	}
};

struct Spacetime_VertexShader_GL4 : public Spacetime_VertexShader {
	Spacetime_VertexShader_GL4() : Spacetime_VertexShader("modern/spacetime/glsl/Spacetime_Vertex.glsl") {}
	Spacetime_VertexShader_GL4(int mode) : Spacetime_VertexShader("modern/spacetime/glsl/Spacetime_Vertex.glsl", mode) {}
};

struct Spacetime_VertexShader_DX11 : public Spacetime_VertexShader {
	Spacetime_VertexShader_DX11() : Spacetime_VertexShader("modern/spacetime/hlsl/Spacetime_Vertex.hlsl") {}
	Spacetime_VertexShader_DX11(int mode) : Spacetime_VertexShader("modern/spacetime/hlsl/Spacetime_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Spacetime_PixelShader : public Modern_PixelShader {
	Spacetime_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Spacetime_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Spacetime_PixelShader_GL4 : public Spacetime_PixelShader {
	Spacetime_PixelShader_GL4() : Spacetime_PixelShader("modern/spacetime/glsl/Spacetime_Frag.glsl") {}
};

struct Spacetime_PixelShader_DX11 : public Spacetime_PixelShader {
	Spacetime_PixelShader_DX11() : Spacetime_PixelShader("modern/spacetime/hlsl/Spacetime_Pixel.hlsl") {}
};

#endif
