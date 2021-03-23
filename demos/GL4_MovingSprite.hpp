#include "native_os_def.h"

#include "FileIO.hpp"
// #include "Input.hpp"
#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "Geo_Construct.hpp"
#include "composites/Humanoid.hpp"

#define MOVE_AMOUNT 16.0

namespace Topl {
	Topl_SceneManager sceneManager;
    // Input_KeyLogger keyLogger;

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
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* rotationBytes_ptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offsetBytes_ptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		ValueGen::appendDataToBytes(rotationBytes_ptr, component->getAngles()->size() * sizeof(float), 2 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(offsetBytes_ptr, component->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		const uint8_t* cameraPosBytes_ptr = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getPos()->data());
		const uint8_t* cameraRotBytes_ptr = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getRotation()->data());
		const uint8_t* matrixBytes_ptr = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getProjMatrix()->data());

		ValueGen::appendDataToBytes(cameraPosBytes_ptr, sMan->getCamera()->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(cameraRotBytes_ptr, sMan->getCamera()->getRotation()->size() * sizeof(float), 1 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(matrixBytes_ptr, sMan->getCamera()->getProjMatrix()->size() * sizeof(float), 0, bytes);

		//ValueGen::assignDataToBytes(matrixBytes, sMan->getCamera()->getProjMatrix()->size() * sizeof(float), bytes);
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

void buttonCallback_w(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(-1 * MOVE_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(0.0f, -1 * MOVE_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { Topl::humanoid.rotate(&Topl::sceneManager, Eigen::Vector3f(1.0f, 0.0f, 0.0f)); } // Rotate