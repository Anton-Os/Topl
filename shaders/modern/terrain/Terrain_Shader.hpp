#ifndef TERRAIN_SHADER_H
#define TERRAIN_SHADER_H

#include "modern/Modern_Shader.hpp"

// Procedural height field terrain with water, snow and contour modes

#include <cmath>

// Vertex Shaders

struct Terrain_VertexShader : public Modern_VertexShader {
	Terrain_VertexShader() : Modern_VertexShader(){ }
	Terrain_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Terrain_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	void setTerrain(float waterLevel, float heightScale, float frequency){ _params[0] = Modern_Float4(waterLevel, heightScale, frequency, 0.0F); }
	float getWaterLevel() const { return _params[0][0]; }

	// CPU mirror of getTerrainHeight() inside Terrain.glsl and Terrain.hlsl, used for walking on the ground
	float getHeight(float x, float z) const {
		float amp = (_params[0][1] > 0.0F)? _params[0][1] : 9.0F;
		float freq = (_params[0][2] > 0.0F)? _params[0][2] : 0.045F;
		float px = x * freq, pz = z * freq;
		float base = fbm(px, pz);
		float ridge = 1.0F - fabs(valueNoise(px * 0.5F + 17.3F, pz * 0.5F + 9.1F) * 2.0F - 1.0F);
		return (base * 0.7F + ridge * ridge * 0.45F - 0.45F) * amp;
	}
	float getGroundHeight(float x, float z) const { float h = getHeight(x, z); return (h > getWaterLevel())? h : getWaterLevel(); }
private:
	static float fract(float v){ return v - floor(v); }
	static float hash12(float x, float y){
		float p3[3] = { fract(x * 0.1031F), fract(y * 0.1031F), fract(x * 0.1031F) };
		float d = p3[0] * (p3[1] + 33.33F) + p3[1] * (p3[2] + 33.33F) + p3[2] * (p3[0] + 33.33F);
		p3[0] += d; p3[1] += d; p3[2] += d;
		return fract((p3[0] + p3[1]) * p3[2]);
	}
	static float valueNoise(float x, float y){
		float ix = floor(x), iy = floor(y);
		float fx = x - ix, fy = y - iy;
		float ux = fx * fx * (3.0F - 2.0F * fx), uy = fy * fy * (3.0F - 2.0F * fy);
		float a = hash12(ix, iy), b = hash12(ix + 1.0F, iy), c = hash12(ix, iy + 1.0F), d = hash12(ix + 1.0F, iy + 1.0F);
		float ab = a + (b - a) * ux, cd = c + (d - c) * ux;
		return ab + (cd - ab) * uy;
	}
	static float fbm(float x, float y){
		float total = 0.0F, amp = 0.5F, norm = 0.0F;
		for(unsigned o = 0; o < 5; o++){
			total += valueNoise(x, y) * amp;
			norm += amp;
			float nx = (0.8F * x - 0.6F * y) * 2.03F;
			float ny = (0.6F * x + 0.8F * y) * 2.03F;
			x = nx; y = ny;
			amp *= 0.5F;
		}
		return total / norm;
	}
};

struct Terrain_VertexShader_GL4 : public Terrain_VertexShader {
	Terrain_VertexShader_GL4() : Terrain_VertexShader("modern/terrain/glsl/Terrain_Vertex.glsl") {}
	Terrain_VertexShader_GL4(int mode) : Terrain_VertexShader("modern/terrain/glsl/Terrain_Vertex.glsl", mode) {}
};

struct Terrain_VertexShader_DX11 : public Terrain_VertexShader {
	Terrain_VertexShader_DX11() : Terrain_VertexShader("modern/terrain/hlsl/Terrain_Vertex.hlsl") {}
	Terrain_VertexShader_DX11(int mode) : Terrain_VertexShader("modern/terrain/hlsl/Terrain_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Terrain_PixelShader : public Modern_PixelShader {
	Terrain_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Terrain_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Terrain_PixelShader_GL4 : public Terrain_PixelShader {
	Terrain_PixelShader_GL4() : Terrain_PixelShader("modern/terrain/glsl/Terrain_Frag.glsl") {}
};

struct Terrain_PixelShader_DX11 : public Terrain_PixelShader {
	Terrain_PixelShader_DX11() : Terrain_PixelShader("modern/terrain/hlsl/Terrain_Pixel.hlsl") {}
};

#endif
