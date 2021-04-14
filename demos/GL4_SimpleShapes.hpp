#include "native_os_def.h"

// #include "FileIO.hpp"
// #include "Input.hpp"
#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "Geo_Construct.hpp"
#include "primitives/Geo_Flat.hpp"
#include "primitives/Geo_Conic.hpp"
#include "primitives/Geo_Extruded.hpp"
#include "primitives/Geo_Sphere3D.hpp" // include for testing!
#include "composites/Chain.hpp"
#include "composites/Grid.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_Scene scene;

	// Primitive Geometry Objects
	Geo_FlatHex hex1 = Geo_FlatHex(0.1f);
	Geo_ExtrudedSquare rect1 = Geo_ExtrudedSquare(0.1f, 0.2f);
	Geo_ConicCircle cone1 = Geo_ConicCircle(0.1f, Eigen::Vector3f(0.0f, 0.0f, 0.5f));
	// Complex Geometry Objects
	Geo_Component chainGeo = Geo_Component((const Geo_RenderObj*)&cone1);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(0.45f); // argument is the distance apart
	Geo_Chain chain("chain", &scene, &chainGeo, &chainProps, 4);
	Geo_Component gridGeo = Geo_Component((const Geo_RenderObj*)&hex1);
	Geo_Grid_Properties gridProps = Geo_Grid_Properties(std::make_pair(3, 0.45f));
	Geo_Grid grid("grid", &scene, &gridGeo, &gridProps);
}

// Shader Objects

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

	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const {
		const uint8_t* cameraPosBytes_ptr = reinterpret_cast<const uint8_t*>(scene->getCamera()->getPos()->data());
		const uint8_t* cameraRotBytes_ptr = reinterpret_cast<const uint8_t*>(scene->getCamera()->getDirection()->data());
		const uint8_t* matrixBytes_ptr = reinterpret_cast<const uint8_t*>(scene->getCamera()->getProjMatrix()->data());

		ValueGen::appendDataToBytes(cameraPosBytes_ptr, scene->getCamera()->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(cameraRotBytes_ptr, scene->getCamera()->getDirection()->size() * sizeof(float), 1 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(matrixBytes_ptr, scene->getCamera()->getProjMatrix()->size() * sizeof(float), 0, bytes);
		return true;
	}
};

struct FragmentShader : public Topl_Shader {
	FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("flatColor", SHDR_uint) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_Scene* const scene, std::vector<uint8_t>* bytes) const { return false; }
};

void buttonCallback_w(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right