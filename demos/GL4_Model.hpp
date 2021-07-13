#include "native_os_def.h"

#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "primitives/Geo_Model3D.hpp"

#define MAX_MODEL_COUNT 1 // hard coded number of model pieces // start with 1 for testing
// #define MAX_MODEL_COUNT 4 // hard coded number of model pieces

namespace Topl {
	Topl_Scene scene;

	// char* fileSource = "C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Models\\OBJFILE.obj"; // hard coded target file
	char* fileSource = "C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Models\\FirstAlien.dae";
    File3D_DocumentTree docTree(fileSource);
	Geo_Model3D model = Geo_Model3D(docTree.getNode(0)); // gets first node
	Geo_Component geo = Geo_Component((const Geo_RenderObj*)&model);
	// std::vector<Geo_Model3D> models;
	// std::vector<Geo_Component> geos; // geometries
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