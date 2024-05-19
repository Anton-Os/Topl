#ifndef TOPL_SHADER_H

#include <string>

#include "Topl_Scene.hpp"

enum SHDR_Type { SHDR_Vertex, SHDR_Pixel, SHDR_Geom, SHDR_TessCtrl, SHDR_TessEval };

enum SHDR_ValueType {
	SHDR_float, SHDR_float_vec2, SHDR_float_vec3, SHDR_float_vec4,
	SHDR_double, SHDR_double_vec2, SHDR_double_vec3, SHDR_double_vec4,
	SHDR_int, SHDR_int_vec2, SHDR_int_vec3, SHDR_int_vec4,
	SHDR_uint, SHDR_uint_vec2, SHDR_uint_vec3, SHDR_uint_vec4,
	// Skip the boolean vector types for now
	SHDR_bool,
	SHDR_matrix_2x2, SHDR_matrix_2x3, SHDR_matrix_2x4, // 2 by matrices
	SHDR_matrix_3x2, SHDR_matrix_3x3, SHDR_matrix_3x4, // 3 by matrices
	SHDR_matrix_4x2, SHDR_matrix_4x3, SHDR_matrix_4x4, // 4 by matrices
};

struct Shader_Type {
	Shader_Type() {}
	Shader_Type(const std::string& n, SHDR_ValueType t) {
		name = n; type = t;
	}
	Shader_Type(const std::string& n, const std::string& s, SHDR_ValueType t) {
		name = n; semantic = s; type = t;
	}
	std::string name;
	std::string semantic = ""; // Relevant for DirectX
	SHDR_ValueType type;
};

#define DEFAULT_SHADER_MODE 0

// Shader

class Topl_Shader {
public:
	Topl_Shader() {} // Blank Constructor
	Topl_Shader(enum SHDR_Type type,std::string fileSrc) { // Filename Constructor
		_shaderType = type;
		_shaderFileSrc = fileSrc;
		_shaderFileSrc = SHADERS_DIR + fileSrc;
		std::replace(_shaderFileSrc.begin(), _shaderFileSrc.end(), '/', '\\');
	}
	enum SHDR_Type getType() const { return _shaderType; }
	const char* getFilePath() const { return _shaderFileSrc.c_str(); }
protected:
	enum SHDR_Type _shaderType;
	std::string _shaderFileSrc; // make into const type!

	std::string genPrefix_glsl() { return "glsl/"; }
	std::string genPrefix_hlsl() { return "hlsl/"; }
};

// Entry shader contains inputs and functionality to pass uniform blocks
class Topl_EntryShader : public Topl_Shader {
public:
	Topl_EntryShader() : Topl_Shader() {} // Blank Constructor

	Topl_EntryShader(std::string fileSrc) : Topl_Shader(SHDR_Vertex, fileSrc) { // Filename Constructor
		_inputs.push_back(Shader_Type("pos", "POSITION", SHDR_float_vec3)); // default pos input
		_inputs.push_back(Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec3)); // default texcoord input
	}

	Topl_EntryShader(std::string fileSrc, std::initializer_list<Shader_Type> inputs) : Topl_Shader(SHDR_Vertex, fileSrc) {  // Inputs Constructor
		for (std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
			_inputs.push_back(*currentInput); // fills input list with data
	}
	const Shader_Type* getInputAtIndex(unsigned index) const { return (index < _inputs.size()) ? &_inputs.at(index) : nullptr; }
	unsigned short getInputCount() const { return _inputs.size(); }

	virtual void genSceneBlock(const Topl_Scene* const scene, const Topl_Camera* const camera, blockBytes_t* bytes) const {
		static Timer_Dynamic dynamic_ticker = Timer_Dynamic(0.0);
		double timeElapse = dynamic_ticker.getAbsSecs();

		// _sceneBlock_bytes.clear();
		appendDataToBytes((uint8_t*)&_mode, sizeof(int), bytes);
		// appendDataToBytes((uint8_t*)&timeElapse, sizeof(double), bytes); // TODO: Uncomment this
		alignDataToBytes((uint8_t*)((camera != nullptr)? camera->getPos() : &_defaultVec), sizeof(Vec3f), 0, bytes);
		alignDataToBytes((uint8_t*)((camera != nullptr)? camera->getZoom() : &_defaultScalar), sizeof(float), 0, bytes);
		appendDataToBytes((uint8_t*)((camera != nullptr)? camera->getRot() : &_defaultVec), sizeof(Vec3f), bytes);
		alignDataToBytes((uint8_t*)((camera != nullptr)? camera->getProjMatrix() : &_defaultMat), sizeof(Mat4x4), 0, bytes);
	}
	virtual void genRenderBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const {
		if(actor != nullptr) if(actor->shaderFunc != nullptr) actor->shaderFunc((Topl_EntryShader*)this);
		// TODO: Else trigger function to reset actor dependent data
		appendDataToBytes((uint8_t*)((actor != nullptr)? actor->getPos() : &_defaultVec), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)((actor != nullptr)? actor->getRot() : &_defaultVec), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)((actor != nullptr)? actor->getSize() : &_defaultVec), sizeof(Vec3f), bytes);
	}

	void setMode(int m){ _mode = m; }
protected:
	int _mode = DEFAULT_SHADER_MODE;
private:
	std::vector<Shader_Type> _inputs; // inputs are required for vertex layout
	Vec3f _defaultVec = VEC_3F_ZERO;
	float _defaultScalar = 1.0f;
	Mat4x4 _defaultMat = MAT_4x4_IDENTITY;
};

typedef const Topl_EntryShader* entry_shader_cptr;
typedef const Topl_Shader* shader_cptr;

#define TOPL_SHADER_H
#endif