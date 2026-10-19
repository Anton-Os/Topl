#ifndef TUNNEL_SHADER_H
#define TUNNEL_SHADER_H

#include "modern/Modern_Shader.hpp"

// Fullscreen raymarched interior of a spinning torus whose axis tilts while it turns
// A point in world space maps into the torus frame with local = rotateY(transpose(tilt) * (world - anchor) + (major, 0, 0), spin)

// Vertex Shaders

struct Tunnel_VertexShader : public Modern_VertexShader {
	Tunnel_VertexShader() : Modern_VertexShader(){ }
	Tunnel_VertexShader(std::string name) : Modern_VertexShader(name) { }
	Tunnel_VertexShader(std::string name, int mode) : Modern_VertexShader(name, mode) { }

	// tilt holds the columns of the torus frame to world rotation, the anchor lies on the centerline
	void setFrame(const Vec3f tilt[3], float spin, float major, float tube, Vec3f anchor, float ribs){
		for(unsigned c = 0; c < 3; c++) _params[c] = Modern_Float4(tilt[c].data[0], tilt[c].data[1], tilt[c].data[2], 0.0F);
		_params[0][3] = spin; _params[1][3] = major; _params[2][3] = tube;
		_params[3] = Modern_Float4(anchor.data[0], anchor.data[1], anchor.data[2], ribs);
	}
	void setDetail(float ribDepth, float pipes, float glow, float panels){ _params[4] = Modern_Float4(ribDepth, pipes, glow, panels); }
	void setLights(float pulseSpeed, float wobble, float hueShift, float headlamp){ _params[5] = Modern_Float4(pulseSpeed, wobble, hueShift, headlamp); }
};

struct Tunnel_VertexShader_GL4 : public Tunnel_VertexShader {
	Tunnel_VertexShader_GL4() : Tunnel_VertexShader("modern/tunnel/glsl/Tunnel_Vertex.glsl") {}
	Tunnel_VertexShader_GL4(int mode) : Tunnel_VertexShader("modern/tunnel/glsl/Tunnel_Vertex.glsl", mode) {}
};

struct Tunnel_VertexShader_DX11 : public Tunnel_VertexShader {
	Tunnel_VertexShader_DX11() : Tunnel_VertexShader("modern/tunnel/hlsl/Tunnel_Vertex.hlsl") {}
	Tunnel_VertexShader_DX11(int mode) : Tunnel_VertexShader("modern/tunnel/hlsl/Tunnel_Vertex.hlsl", mode) {}
};

// Pixel Shaders

struct Tunnel_PixelShader : public Modern_PixelShader {
	Tunnel_PixelShader() : Modern_PixelShader(){ } // Blank Constructor
	Tunnel_PixelShader(std::string name) : Modern_PixelShader(name){ }
};

struct Tunnel_PixelShader_GL4 : public Tunnel_PixelShader {
	Tunnel_PixelShader_GL4() : Tunnel_PixelShader("modern/tunnel/glsl/Tunnel_Frag.glsl") {}
};

struct Tunnel_PixelShader_DX11 : public Tunnel_PixelShader {
	Tunnel_PixelShader_DX11() : Tunnel_PixelShader("modern/tunnel/hlsl/Tunnel_Pixel.hlsl") {}
};

#endif
