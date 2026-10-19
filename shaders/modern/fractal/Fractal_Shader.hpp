#ifndef FRACTAL_SHADER_H
#define FRACTAL_SHADER_H

#include "modern/Modern_Shader.hpp"

// Fullscreen raymarched 3D fractals: mandelbulb, menger sponge, sierpinski tetrahedron, mandelbox and quaternion julia

enum FRACTAL_Type {
	FRACTAL_Mandelbulb = 0,
	FRACTAL_Menger = 1,
	FRACTAL_Sierpinski = 2,
	FRACTAL_Mandelbox = 3,
	FRACTAL_Julia = 4,
	FRACTAL_COUNT = 5
};

// Vertex Shaders

struct Fractal_VertexShader : public Modern_VertexShader {
	Fractal_VertexShader() : Modern_VertexShader(){ }
	Fractal_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Fractal_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	// shape is the bulb power or the box scale, worldScale sizes the fractal in world units, bound is its radius in fractal units
	void setFractal(FRACTAL_Type type, float shape, unsigned iterations, float worldScale, float bound){
		_params[0] = Modern_Float4((float)type, shape, (float)iterations, worldScale);
		_params[1][3] = bound;
	}
	void setLook(float colorShift, float glow, float detail){ _params[1][0] = colorShift; _params[1][1] = glow; _params[1][2] = detail; }
	void setMotion(float spin, float tilt, float shadow, float fold){ _params[2] = Modern_Float4(spin, tilt, shadow, fold); }
	void setJulia(Modern_Float4 constant){ _params[3] = constant; }
};

struct Fractal_VertexShader_GL4 : public Fractal_VertexShader {
	Fractal_VertexShader_GL4() : Fractal_VertexShader("modern/fractal/glsl/Fractal_Vertex.glsl") {}
	Fractal_VertexShader_GL4(int mode) : Fractal_VertexShader("modern/fractal/glsl/Fractal_Vertex.glsl", mode) {}
};

struct Fractal_VertexShader_DX11 : public Fractal_VertexShader {
	Fractal_VertexShader_DX11() : Fractal_VertexShader("modern/fractal/hlsl/Fractal_Vertex.hlsl") {}
	Fractal_VertexShader_DX11(int mode) : Fractal_VertexShader("modern/fractal/hlsl/Fractal_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Fractal_PixelShader : public Modern_PixelShader {
	Fractal_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Fractal_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Fractal_PixelShader_GL4 : public Fractal_PixelShader {
	Fractal_PixelShader_GL4() : Fractal_PixelShader("modern/fractal/glsl/Fractal_Frag.glsl") {}
};

struct Fractal_PixelShader_DX11 : public Fractal_PixelShader {
	Fractal_PixelShader_DX11() : Fractal_PixelShader("modern/fractal/hlsl/Fractal_Pixel.hlsl") {}
};

#endif
