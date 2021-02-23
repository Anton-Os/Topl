#include "native_os_def.h"

#include "FileIO.hpp"
#include "Input.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "Geo_Construct.hpp"
#include "composites/Humanoid.hpp"

#define MOVE_AMOUNT 16.0

namespace Topl {
	Topl_SceneManager sceneManager;
    Input_KeyLogger keyLogger;

	// TODO: Make these not device specific, relative file paths only!
	std::pair<const char*, Eigen::Vector3f> humanoidProps[HUMANOID_PARTS_COUNT] = {
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png", Eigen::Vector3f(0.0f, 0.11f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftArm.png", Eigen::Vector3f(0.0f, -0.1f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightArm.png", Eigen::Vector3f(0.12f, -0.14f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Body.png", Eigen::Vector3f(-0.12f, -0.14f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftLeg.png", Eigen::Vector3f(0.06f, -0.35f, 0.0)),
		std::make_pair("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightLeg.png", Eigen::Vector3f(-0.06f, -0.35f, 0.0))
	};

	// Geo_Humanoid humanoid;
	Geo_Humanoid humanoid("humanoid", &sceneManager, humanoidProps, 0.25f);
}

struct VertexShader : public Topl_Shader {
	VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		const uint8_t* rotationBytesPtr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offsetBytesPtr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		bytes->assign({ 
			*(rotationBytesPtr + 0), *(rotationBytesPtr + 1), *(rotationBytesPtr + 2), *(rotationBytesPtr + 3), // Main axis rotation
			*(rotationBytesPtr + 4), *(rotationBytesPtr + 5), *(rotationBytesPtr + 6), *(rotationBytesPtr + 7), // Cross axis rotation
			0, 0, 0, 0, // 1 byte padding
			0, 0, 0, 0, // 1 byte padding
			*(offsetBytesPtr + 0), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), *(offsetBytesPtr + 3), // X offset value
			*(offsetBytesPtr + 4), *(offsetBytesPtr + 5), *(offsetBytesPtr + 6), *(offsetBytesPtr + 7), // Y offset value
			*(offsetBytesPtr + 8), *(offsetBytesPtr + 9), *(offsetBytesPtr + 10), *(offsetBytesPtr + 11), // Z offset value
			0, 0, 0, 0 // 1 byte padding
		});

		return true; // Indicates that an implementation exists
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		const uint8_t* matrixBytes = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getProjMatrix()->data());

		/* bytes->assign({ 0xFF }); */

		// Assign with camera value
		bytes->assign({
			*(matrixBytes + 0), *(matrixBytes + 1), *(matrixBytes + 2), *(matrixBytes + 3),
			*(matrixBytes + 4), *(matrixBytes + 5), *(matrixBytes + 6), *(matrixBytes + 7),
			*(matrixBytes + 8), *(matrixBytes + 9), *(matrixBytes + 10), *(matrixBytes + 11),
			*(matrixBytes + 12), *(matrixBytes + 13), *(matrixBytes + 14), *(matrixBytes + 15),

			*(matrixBytes + 16), *(matrixBytes + 17), *(matrixBytes + 18), *(matrixBytes + 19),
			*(matrixBytes + 20), *(matrixBytes + 21), *(matrixBytes + 22), *(matrixBytes + 23),
			*(matrixBytes + 24), *(matrixBytes + 25), *(matrixBytes + 26), *(matrixBytes + 27),
			*(matrixBytes + 28), *(matrixBytes + 29), *(matrixBytes + 30), *(matrixBytes + 31),

			*(matrixBytes + 32), *(matrixBytes + 33), *(matrixBytes + 34), *(matrixBytes + 35),
			*(matrixBytes + 36), *(matrixBytes + 37), *(matrixBytes + 38), *(matrixBytes + 39),
			*(matrixBytes + 40), *(matrixBytes + 41), *(matrixBytes + 42), *(matrixBytes + 43),
			*(matrixBytes + 44), *(matrixBytes + 45), *(matrixBytes + 46), *(matrixBytes + 47),

			*(matrixBytes + 48), *(matrixBytes + 49), *(matrixBytes + 50), *(matrixBytes + 51),
			*(matrixBytes + 52), *(matrixBytes + 53), *(matrixBytes + 54), *(matrixBytes + 55),
			*(matrixBytes + 56), *(matrixBytes + 57), *(matrixBytes + 58), *(matrixBytes + 59),
			*(matrixBytes + 60), *(matrixBytes + 61), *(matrixBytes + 62), *(matrixBytes + 63),
			});

		return true;
	}
};

struct FragmentShader : public Topl_Shader {
	FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const override { return false; }                                                                       
};