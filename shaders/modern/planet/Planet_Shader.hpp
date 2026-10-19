#ifndef PLANET_SHADER_H
#define PLANET_SHADER_H

#include "modern/Modern_Shader.hpp"

// Planet surface displaced from a unit sphere with evolving 3D noise terrain, oceans, biomes and quake rings
// getTerrain() mirrors planetTerrain() inside Planet.glsl and Planet.hlsl so the CPU can walk on the surface

// Vertex Shaders

struct Planet_VertexShader : public Modern_VertexShader {
	Planet_VertexShader() : Modern_VertexShader(){ }
	Planet_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Planet_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	// rows of the planet frame to world rotation
	void setPlanet(const Vec3f rows[3], float radius, float heightScale, float seaLevel){
		for(unsigned r = 0; r < 3; r++) _params[r] = Modern_Float4(rows[r].data[0], rows[r].data[1], rows[r].data[2], 0.0F);
		_params[0][3] = radius; _params[1][3] = heightScale; _params[2][3] = seaLevel;
	}
	void setTerrain(float time, float frequency){ _params[3][0] = time; _params[3][1] = frequency; }
	void setPulse(float age, float strength, Vec3f origin){ // negative age disables the pulse
		_params[3][2] = age; _params[3][3] = strength;
		_params[4] = Modern_Float4(origin.data[0], origin.data[1], origin.data[2], _params[4][3]);
	}
	void setBiome(unsigned biome){ _params[4][3] = (float)(biome % 4); }
	void setCenter(Vec3f center, float waves){ _params[5] = Modern_Float4(center.data[0], center.data[1], center.data[2], waves); }

	float getRadius() const { return _params[0][3]; }
	float getHeightScale() const { return (_params[1][3] > 0.001F)? _params[1][3] : 0.001F; }
	float getSeaLevel() const { return _params[2][3]; }

	float getTerrain(const float dir[3]) const { // height above the base radius for a unit direction in the planet frame
		float t = _params[3][0], f = _params[3][1];
		float drift[3] = { (float)sin(t * 0.11F) * 0.35F, (float)cos(t * 0.07F) * 0.35F, (float)sin(t * 0.05F + 1.3F) * 0.35F };
		float p1[3], p2[3];
		for(unsigned c = 0; c < 3; c++){ p1[c] = dir[c] * f + drift[c]; p2[c] = dir[c] * f * 3.1F - drift[c] * 1.7F; }
		float continents = fbm3(p1) * 2.0F - 1.0F;
		float mountains = ridged3(p2);
		float h = (continents * 1.4F + mountains * smoothstep(0.0F, 0.3F, continents) * 1.1F) * getHeightScale();

		if(_params[3][2] >= 0.0F){
			float d = dir[0] * _params[4][0] + dir[1] * _params[4][1] + dir[2] * _params[4][2];
			float angle = (float)acos((d < -1.0F)? -1.0F : (d > 1.0F)? 1.0F : d);
			float front = (angle - _params[3][2] * 0.25F) * getRadius() / 6.0F;
			h += (float)exp(-front * front) * _params[3][3] * (float)exp(-_params[3][2] * 0.3F);
		}
		return h;
	}
private:
	static float fract(float x){ return x - (float)floor(x); }
	static float smoothstep(float a, float b, float x){ float t = (x - a) / (b - a); t = (t < 0.0F)? 0.0F : (t > 1.0F)? 1.0F : t; return t * t * (3.0F - 2.0F * t); }
	static float hash13(const float p[3]){
		float p3[3] = { fract(p[0] * 0.1031F), fract(p[1] * 0.1031F), fract(p[2] * 0.1031F) };
		float d = p3[0] * (p3[2] + 31.32F) + p3[1] * (p3[1] + 31.32F) + p3[2] * (p3[0] + 31.32F);
		for(unsigned c = 0; c < 3; c++) p3[c] += d;
		return fract((p3[0] + p3[1]) * p3[2]);
	}
	static float noise3(const float p[3]){
		float i[3], u[3], n[8];
		for(unsigned c = 0; c < 3; c++){
			i[c] = (float)floor(p[c]);
			float f = p[c] - i[c];
			u[c] = f * f * (3.0F - 2.0F * f);
		}
		for(unsigned k = 0; k < 8; k++){
			float corner[3] = { i[0] + (float)(k & 1), i[1] + (float)((k >> 1) & 1), i[2] + (float)((k >> 2) & 1) };
			n[k] = hash13(corner);
		}
		float x00 = n[0] + (n[1] - n[0]) * u[0], x10 = n[2] + (n[3] - n[2]) * u[0];
		float x01 = n[4] + (n[5] - n[4]) * u[0], x11 = n[6] + (n[7] - n[6]) * u[0];
		float y0 = x00 + (x10 - x00) * u[1], y1 = x01 + (x11 - x01) * u[1];
		return y0 + (y1 - y0) * u[2];
	}
	static void rotateOctave(float p[3], float scale){
		float x = p[0], y = p[1], z = p[2];
		p[0] = (0.80F * y + 0.60F * z) * scale;
		p[1] = (-0.80F * x + 0.36F * y - 0.48F * z) * scale;
		p[2] = (-0.60F * x - 0.48F * y + 0.64F * z) * scale;
	}
	static float fbm3(const float start[3]){
		float p[3] = { start[0], start[1], start[2] };
		float total = 0.0F, amp = 0.5F, norm = 0.0F;
		for(unsigned o = 0; o < 5; o++){ total += noise3(p) * amp; norm += amp; rotateOctave(p, 2.03F); amp *= 0.5F; }
		return total / norm;
	}
	static float ridged3(const float start[3]){
		float p[3] = { start[0], start[1], start[2] };
		float total = 0.0F, amp = 0.5F, norm = 0.0F;
		for(unsigned o = 0; o < 4; o++){
			float n = 1.0F - (float)fabs(noise3(p) * 2.0F - 1.0F);
			total += n * n * amp; norm += amp; rotateOctave(p, 2.11F); amp *= 0.5F;
		}
		return total / norm;
	}
};

struct Planet_VertexShader_GL4 : public Planet_VertexShader {
	Planet_VertexShader_GL4() : Planet_VertexShader("modern/planet/glsl/Planet_Vertex.glsl") {}
	Planet_VertexShader_GL4(int mode) : Planet_VertexShader("modern/planet/glsl/Planet_Vertex.glsl", mode) {}
};

struct Planet_VertexShader_DX11 : public Planet_VertexShader {
	Planet_VertexShader_DX11() : Planet_VertexShader("modern/planet/hlsl/Planet_Vertex.hlsl") {}
	Planet_VertexShader_DX11(int mode) : Planet_VertexShader("modern/planet/hlsl/Planet_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Planet_PixelShader : public Modern_PixelShader {
	Planet_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Planet_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Planet_PixelShader_GL4 : public Planet_PixelShader {
	Planet_PixelShader_GL4() : Planet_PixelShader("modern/planet/glsl/Planet_Frag.glsl") {}
};

struct Planet_PixelShader_DX11 : public Planet_PixelShader {
	Planet_PixelShader_DX11() : Planet_PixelShader("modern/planet/hlsl/Planet_Pixel.hlsl") {}
};

#endif
