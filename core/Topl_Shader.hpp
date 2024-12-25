#ifndef TOPL_SHADER_H

#include <string>
#include <vector>

#include "scene/Topl_Scene.hpp"

enum SHDR_Type { SHDR_Vertex, SHDR_Pixel, SHDR_Geom, SHDR_TessCtrl, SHDR_TessEval, SHDR_Compute };

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
#define NO_PADDING 0
#define PADDING_WIDTH 16 // padding aligned by 4 byte boundaries
#define MAX_SHADER_INPUTS 16

typedef std::vector<uint8_t> blockBytes_t; // container used to pass data to shaders
typedef const uint8_t* bytes_cptr; // intermediate type for all shader data

// Shader

class Topl_Shader {
public:
	Topl_Shader() {} // Blank Constructor
	Topl_Shader(enum SHDR_Type type,std::string fileSrc) { // Filename Constructor
		_shaderType = type;
		_shaderFilePath = fileSrc;
		_shaderFilePath = SHADERS_DIR + fileSrc;
#ifdef _WIN32
		std::replace(_shaderFilePath.begin(), _shaderFilePath.end(), '/', '\\');
#endif
	}
	enum SHDR_Type getType() const { return _shaderType; }
	std::string getFilePath() const { return _shaderFilePath; }
	std::string getFileSource() const {
		std::string shaderSrc = readFile(_shaderFilePath.c_str());

		unsigned short startOffset = 0, includeOffset = 0;
		while(shaderSrc.find("#include", includeOffset) != std::string::npos){
			// TODO: Check the include type and replace conditionally based on <> or ""

			startOffset = shaderSrc.find("#include", includeOffset);
			includeOffset += startOffset + 10; // location of include after the space

			std::string includeStr = "";
			while(shaderSrc[includeOffset] != '\n' && shaderSrc[includeOffset] != '\0'){
				if(shaderSrc[includeOffset] != '\"' && shaderSrc[includeOffset] != '>' && shaderSrc[includeOffset] != ';') includeStr += shaderSrc[includeOffset];
				includeOffset++;
			}

			if(includeStr.substr(includeStr.size() - 4) == "glsl") includeStr = SHADERS_DIR + genPrefix_glsl() + includeStr;
			else if(includeStr.substr(includeStr.size() - 4) == "hlsl") includeStr = SHADERS_DIR + genPrefix_hlsl() + includeStr;

			std::string includeSrc = readFile(includeStr.c_str());
			if(includeSrc.empty()) if(_embedMap.find(includeStr) != _embedMap.end()) includeSrc = _embedMap.at(includeStr);
			shaderSrc.replace(startOffset, includeOffset - startOffset, includeSrc);
		}
		return shaderSrc;
	}
protected:
	enum SHDR_Type _shaderType;
	std::string _shaderFilePath; // make into const type!
	std::map<std::string, std::string> _embedMap;

	std::string genPrefix_glsl() const { return "glsl/"; }
	std::string genPrefix_hlsl() const { return "hlsl/"; }

	void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes) const {
		targetBytes->clear();
		targetBytes->resize(dataSize);
		
		for(unsigned d = 0; d < dataSize; d++) if(data_ptr + d != nullptr) targetBytes->at(d) = *(data_ptr + d);
	}

	void alignDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingSize, std::vector<uint8_t>* targetBytes) const {
		for (unsigned d = 0; d < dataSize + paddingSize; d++)
			(d < dataSize && data_ptr + d != nullptr)
				? targetBytes->push_back(*(data_ptr + d)) // value is copied into targetBytes
				: targetBytes->push_back(0); // otherwise zero padding is applied
	}

	void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes) const {
		size_t paddingSize = PADDING_WIDTH - (dataSize % PADDING_WIDTH); // manually computed padding value
		alignDataToBytes(data_ptr, dataSize, paddingSize, targetBytes);
	}

	void embed(const std::string& embedTag, const std::string& embedStr){ _embedMap.insert({ embedTag, embedStr }); }
};

// Entry shader contains inputs and functionality to pass uniform blocks
class Topl_EntryShader : public Topl_Shader {
public:
	Topl_EntryShader() : Topl_Shader() {} // Blank Constructor

	Topl_EntryShader(std::string fileSrc) : Topl_Shader(SHDR_Vertex, fileSrc) { // Filename Constructor
		_inputs.push_back(Shader_Type("pos", "POSITION", SHDR_float_vec3)); // default pos input
		_inputs.push_back(Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec3)); // default texcoord input
		_inputs.push_back(Shader_Type("normal", "NORMAL", SHDR_float_vec3));
		_inputs.push_back(Shader_Type("vert_color", "COLOR", SHDR_float_vec3));
	}

	Topl_EntryShader(std::string fileSrc, std::initializer_list<Shader_Type> inputs) : Topl_Shader(SHDR_Vertex, fileSrc) {  // Inputs Constructor
		for (std::initializer_list<Shader_Type>::iterator currentInput = inputs.begin(); currentInput < inputs.end(); currentInput++)
			_inputs.push_back(*currentInput); // fills input list with data
	}
	const Shader_Type* getInputAtIndex(unsigned index) const { return (index < _inputs.size()) ? &_inputs.at(index) : nullptr; }
	unsigned short getInputCount() const { return _inputs.size(); }

	virtual void genSceneBlock(scene_cptr scene, blockBytes_t* bytes) const {
		static Timer_Dynamic dynamic_ticker = Timer_Dynamic(0.0);
		Topl_Camera* camera = scene->camera;
		// double timeElapse = dynamic_ticker.getAbsSecs();

		// _sceneBlock_bytes.clear();
		appendDataToBytes((uint8_t*)&_mode, sizeof(int), bytes);
		// appendDataToBytes((uint8_t*)&timeElapse, sizeof(double), bytes); // TODO: Uncomment this
		alignDataToBytes((uint8_t*)((camera != nullptr)? camera->getPos() : &_defaultVec), sizeof(Vec3f), 0, bytes);
		alignDataToBytes((uint8_t*)((camera != nullptr)? camera->getZoom() : &_defaultScalar), sizeof(float), 0, bytes);
		appendDataToBytes((uint8_t*)((camera != nullptr)? camera->getRot() : &_defaultVec), sizeof(Vec3f), bytes);
		alignDataToBytes((uint8_t*)((camera != nullptr)? camera->getProjMatrix() : &_defaultMat), sizeof(Mat4x4), 0, bytes);
	}

	virtual void genActorBlock(const Geo_Actor* const actor, blockBytes_t* bytes) const {
		if(actor != nullptr) if(actor->shaderFunc != nullptr) actor->shaderFunc();
		// TODO: Else trigger function to reset actor dependent data
		appendDataToBytes((uint8_t*)((actor != nullptr)? actor->getPos() : &_defaultVec), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)((actor != nullptr)? actor->getRot() : &_defaultVec), sizeof(Vec3f), bytes);
		appendDataToBytes((uint8_t*)((actor != nullptr)? actor->getSize() : &_defaultVec), sizeof(Vec3f), bytes);
	}

	virtual void genMeshBlock(const Geo_Mesh* const mesh, blockBytes_t* bytes) const {
		unsigned vertexCount = mesh->getVertexCount();
		unsigned instanceCount = mesh->getInstanceCount();
		unsigned tessLevel = mesh->getTessLevel();
        // Vec3f origin = mesh->getOrigin();

        appendDataToBytes((uint8_t*)((mesh != nullptr)? (uint8_t*)&vertexCount : &_defaultNum), sizeof(unsigned), bytes);
		appendDataToBytes((uint8_t*)((mesh != nullptr)? (uint8_t*)&instanceCount : &_defaultNum), sizeof(unsigned), bytes);
		appendDataToBytes((uint8_t*)((mesh != nullptr)? (uint8_t*)&mesh->drawMode : &_defaultNum), sizeof(unsigned), bytes);
		appendDataToBytes((uint8_t*)((mesh != nullptr)? (uint8_t*)&tessLevel : &_defaultNum), sizeof(unsigned), bytes);
        // alignDataToBytes((uint8_t*)((mesh != nullptr)? &origin : &_defaultVec), sizeof(Vec3f), 4, bytes); // origin
		// appendDataToBytes((uint8_t*)((actor != nullptr)? &mesh->getBounds() : &_defaultMat), sizeof(Vec3f) * 6, bytes); // bounds

		if(mesh != nullptr) appendDataToBytes((uint8_t*)mesh->getInstanceData(), sizeof(Mat4x4) * mesh->getInstanceCount(), bytes);
	}

	virtual void reset(){ _mode = DEFAULT_SHADER_MODE; }

	void setMode(int m){ _mode = m; }
protected:
	int _mode = DEFAULT_SHADER_MODE;
private:
	std::vector<Shader_Type> _inputs; // inputs are required for vertex layout
	Vec3f _defaultVec = VEC_3F_ZERO;
	uint8_t _defaultNum = 1;
	float _defaultScalar = 1.0f;
	Mat4x4 _defaultMat = MAT_4x4_IDENTITY;
};

typedef const Topl_EntryShader* entry_shader_cptr;
typedef const Topl_Shader* shader_cptr;

#define TOPL_SHADER_H
#endif
