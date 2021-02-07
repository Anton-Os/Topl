#include "native_os_def.h"

#include "FileIO.hpp"

#include "Topl_Renderer_Drx11.hpp"

// #include "Geo_Construct.hpp"
#include "primitives/Geo_Sphere2D.hpp"
#include "primitives/Geo_Rect3D.hpp"
#include "composites/Chain.hpp"
#include "composites/Grid.hpp"

#define MOVE_AMOUNT 8.0

namespace Topl {
	// Management Objects
	Topl_SceneManager sceneManager;

	// Primitive Geometry Objects
	Geo_Sphere2D sphere1 = Geo_Sphere2D(0.1f, 30);
	Geo_Rect3D box1 = Geo_Rect3D(0.4f);
	// Composite Geometry Objects
	Geo_Component chainGeo = Geo_Component((const Geo_RenderObj*)&sphere1);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(0.3f); // 0.1f is the distance apart
	Geo_Chain chain("chain", &sceneManager, &chainGeo, &chainProps, 4);
	Geo_Component gridGeo = Geo_Component((const Geo_RenderObj*)&box1);
	Geo_Grid_Properties gridProps = Geo_Grid_Properties(std::make_pair(3, 0.4f));
	Geo_Grid grid("grid", &sceneManager, &gridGeo, &gridProps);
}

struct VertexShader : public Topl_Shader {
	VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3), Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) } // Inputs
		) {  }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		const uint8_t* offsetBytesPtr = reinterpret_cast<const uint8_t*>(component->getPos()->data());
		const uint8_t* rotationBytesPtr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());

		bytes->assign({
			*(offsetBytesPtr), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), 0, // Gets offset values for x, y, and z, 0 for padding
			*(rotationBytesPtr), *(rotationBytesPtr + 1), 0, 0 // Gets rotations values on x and y axis, 0's for padding
		});

		bytes->assign({
			*(offsetBytesPtr + 0), *(offsetBytesPtr + 1), *(offsetBytesPtr + 2), *(offsetBytesPtr + 3), // X offset value
			*(offsetBytesPtr + 4), *(offsetBytesPtr + 5), *(offsetBytesPtr + 6), *(offsetBytesPtr + 7), // Y offset value
			*(offsetBytesPtr + 8), *(offsetBytesPtr + 9), *(offsetBytesPtr + 10), *(offsetBytesPtr + 11), // Z offset value
			0, 0, 0, 0, // 0 byte padding
			*(rotationBytesPtr + 0), *(rotationBytesPtr + 1), *(rotationBytesPtr + 2), *(rotationBytesPtr + 3), // Main axis rotation
			*(rotationBytesPtr + 4), *(rotationBytesPtr + 5), *(rotationBytesPtr + 6), *(rotationBytesPtr + 7), // Cross axis rotation
			0, 0, 0, 0, // 0 byte padding
			0, 0, 0, 0 // 0 byte padding
		});

		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false; // No implementation
	}
};

struct PixelShader : public Topl_Shader {
	PixelShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3), Shader_Type("texcoord", "TEXCOORD", SHDR_float_vec2) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		return false; // No implementation
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false; // No implementation
	}
};
