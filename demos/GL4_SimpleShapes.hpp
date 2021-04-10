#include "native_os_def.h"

<<<<<<< refs/remotes/origin/linux_port
#include "FileIO.hpp"
#include "Input.hpp"

#include "Topl_Renderer_GL4.hpp"

// #include "Geo_Construct.hpp"
#include "primitives/Geo_Sphere2D.hpp"
#include "primitives/Geo_Rect3D.hpp"
=======
// #include "FileIO.hpp"
// #include "Input.hpp"
#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "Geo_Construct.hpp"
#include "primitives/Geo_NGon2D.hpp"
#include "primitives/Geo_Box3D.hpp"
>>>>>>> local
#include "composites/Chain.hpp"
#include "composites/Grid.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_SceneManager sceneManager;

	// Primitive Geometry Objects
	Geo_NGon2D sphere1 = Geo_NGon2D(0.1f, 30);
	Geo_Box3D box1 = Geo_Box3D(0.4f);
	// Composite Geometry Objects
	Geo_Component chainGeo = Geo_Component((const Geo_RenderObj*)&sphere1);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(0.3f); // 0.1f is the distance apart
	Geo_Chain chain("chain", &sceneManager, &chainGeo, &chainProps, 4);
	Geo_Component gridGeo = Geo_Component((const Geo_RenderObj*)&box1);
	Geo_Grid_Properties gridProps = Geo_Grid_Properties(std::make_pair(3, 0.4f));
	Geo_Grid grid("grid", &sceneManager, &gridGeo, &gridProps);
}

// Shader Objects

struct VertexShader : public Topl_Shader {
	VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3), Shader_Type("texcoord", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
<<<<<<< refs/remotes/origin/linux_port
		const uint8_t* rotationBytesPtr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offsetBytesPtr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		bytes->assign({ 
			*(rotationBytesPtr + 0), *(rotationBytesPtr + 1), *(rotationBytesPtr + 2), *(rotationBytesPtr + 3), // Main axis rotation
			*(rotationBytesPtr + 4), *(rotationBytesPtr + 5), *(rotationBytesPtr + 6), *(rotationBytesPtr + 7), // Cross axis rotation
			0, 0, 0, 0, // 0 byte padding
			0, 0, 0, 0, // 0 byte padding
			*(offsetBytesPtr + 0), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), *(offsetBytesPtr + 3), // X offset value
			*(offsetBytesPtr + 4), *(offsetBytesPtr + 5), *(offsetBytesPtr + 6), *(offsetBytesPtr + 7), // Y offset value
			*(offsetBytesPtr + 8), *(offsetBytesPtr + 9), *(offsetBytesPtr + 10), *(offsetBytesPtr + 11), // Z offset value
			0, 0, 0, 0 // 0 byte padding
		});

		return true; // Indicates that an implementation exists
=======
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* rotationBytes_ptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offsetBytes_ptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		ValueGen::appendDataToBytes(rotationBytes_ptr, component->getAngles()->size() * sizeof(float), 2 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(offsetBytes_ptr, component->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		return true;
>>>>>>> local
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		const uint8_t* cameraPosBytes_ptr = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getPos()->data());
		const uint8_t* cameraRotBytes_ptr = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getDirection()->data());
		const uint8_t* matrixBytes_ptr = reinterpret_cast<const uint8_t*>(sMan->getCamera()->getProjMatrix()->data());

		ValueGen::appendDataToBytes(cameraPosBytes_ptr, sMan->getCamera()->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(cameraRotBytes_ptr, sMan->getCamera()->getDirection()->size() * sizeof(float), 1 * sizeof(float), bytes);
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
	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const { return false; }
};

void buttonCallback_w(void) { Topl::sceneManager.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::sceneManager.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::sceneManager.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::sceneManager.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right