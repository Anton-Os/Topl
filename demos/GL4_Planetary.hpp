#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "primitives/Geo_Sphere.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_Scene scene;

	NGon3D ngon = { 0.2f, 40, 41 };
	Geo_SphereUV sphereGeo(ngon);
	Geo_Component sphere((const Geo_RenderObj*)&sphereGeo);
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