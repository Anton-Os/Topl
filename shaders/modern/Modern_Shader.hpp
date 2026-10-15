#ifndef MODERN_SHADER_H
#define MODERN_SHADER_H

#include <cmath>
#include <cstring>

#include "Topl_Pipeline.hpp"

// Modern shaders share one actor block and one scene block layout so any modern pipeline can draw any scene
// The FPS camera is evaluated inside every vertex shader from the camera position, angles and lens values below

#define MODERN_PARAMS_COUNT 8 // generic float4 slots available to each pipeline
#define MODERN_FOV 1.0472F // 60 degree vertical field of view
#define MODERN_NEAR 0.05F
#define MODERN_FAR 400.0F

struct Modern_Float4 {
	Modern_Float4(){}
	Modern_Float4(float x, float y, float z, float w){ data[0] = x; data[1] = y; data[2] = z; data[3] = w; }

	float& operator [](unsigned short i){ return data[i % 4]; }
	float operator [](unsigned short i) const { return data[i % 4]; }

	float data[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
};

// Values shared by every modern pipeline, written once per frame by the runtime
struct Modern_SceneState {
	Modern_Float4 lens = Modern_Float4(MODERN_FOV, 1.0F, MODERN_NEAR, MODERN_FAR); // fov, aspect, near, far
	Modern_Float4 time; // elapsed seconds, frame seconds, frame count
	Modern_Float4 lightPos = Modern_Float4(0.45F, 0.6F, 0.35F, -1.0F); // negative w is a directional light
	Modern_Float4 lightColor = Modern_Float4(1.0F, 0.95F, 0.85F, 0.45F); // w is ambient strength
	Modern_Float4 fog = Modern_Float4(0.62F, 0.72F, 0.85F, 0.012F); // w is fog density
};

// Values unique to each actor
struct Modern_ActorProps {
	Modern_Float4 color = Modern_Float4(1.0F, 1.0F, 1.0F, 1.0F);
	Modern_Float4 params;
	Modern_Float4 tex = Modern_Float4(0.0F, 1.0F, 0.0F, 0.0F); // texture blend, tiling, scroll speed, mapping
};

// Vertex Shader

class Modern_VertexShader : public Topl_EntryShader {
public:
	Modern_VertexShader() : Topl_EntryShader(){ }
	Modern_VertexShader(std::string name) : Topl_EntryShader(name) { }
	Modern_VertexShader(std::string name, int mode) : Topl_EntryShader(name) { _mode = mode; }

	static Modern_SceneState& getState(){ static Modern_SceneState state; return state; }
	static std::map<const Geo_Actor*, Modern_ActorProps>& getActorProps(){ static std::map<const Geo_Actor*, Modern_ActorProps> props; return props; }
	static void setActorColor(const Geo_Actor* actor, Modern_Float4 color){ getActorProps()[actor].color = color; }
	static void setActorParams(const Geo_Actor* actor, Modern_Float4 params){ getActorProps()[actor].params = params; }
	static void setActorTexture(const Geo_Actor* actor, Modern_Float4 tex){ getActorProps()[actor].tex = tex; }
	static const Geo_Actor*& getHighlight(){ static const Geo_Actor* highlight = nullptr; return highlight; } // selected actor pulses

	void setParam(unsigned index, Modern_Float4 value){ if(index < MODERN_PARAMS_COUNT) _params[index] = value; }
	Modern_Float4 getParam(unsigned index) const { return (index < MODERN_PARAMS_COUNT)? _params[index] : Modern_Float4(); }
	int getMode() const { return _mode; }

	void genSceneBlock(const Topl_Scene* const scene, blockBytes_t* bytes) const override {
		const Modern_SceneState& state = getState();

		Topl_EntryShader::genSceneBlock(scene, bytes); // mode, camera position and zoom, camera angles, projection
		alignDataToBytes((const uint8_t*)state.lens.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		alignDataToBytes((const uint8_t*)state.time.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		alignDataToBytes((const uint8_t*)state.lightPos.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		alignDataToBytes((const uint8_t*)state.lightColor.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		alignDataToBytes((const uint8_t*)state.fog.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		for(unsigned p = 0; p < MODERN_PARAMS_COUNT; p++)
			alignDataToBytes((const uint8_t*)_params[p].data, sizeof(Modern_Float4), NO_PADDING, bytes);
	}

	void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const override {
		Modern_ActorProps props;
		auto entry = getActorProps().find(actor);
		if(entry != getActorProps().end()) props = entry->second;
		if(actor != nullptr && actor == getHighlight()){ // selected actors pulse brighter
			float pulse = 1.25F + 0.35F * (float)sin(getState().time[0] * 6.0F);
			for(unsigned c = 0; c < 3; c++) props.color[c] *= pulse;
		}

		Topl_EntryShader::genActorBlock(actor, bytes); // offset, rotation, scale
		alignDataToBytes((const uint8_t*)props.color.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		alignDataToBytes((const uint8_t*)props.params.data, sizeof(Modern_Float4), NO_PADDING, bytes);
		alignDataToBytes((const uint8_t*)props.tex.data, sizeof(Modern_Float4), NO_PADDING, bytes);
	}
protected:
	Modern_Float4 _params[MODERN_PARAMS_COUNT];
};

// Pixel Shader

struct Modern_PixelShader : public Topl_Shader {
	Modern_PixelShader() : Topl_Shader(){ } // Blank Constructor
	Modern_PixelShader(std::string name) : Topl_Shader(SHDR_Pixel, name){ }
};

#endif
