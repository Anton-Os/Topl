#include "native_os_def.h"

#include "FileIO.hpp"
#include "Topl_Renderer_GL4.hpp"

#include "Geo_Construct.hpp"
#include "primitives/Geo_Sphere2D.hpp"
#include "primitives/Geo_Rect3D.hpp"
#include "composites/Chain.hpp"
#include "composites/Grid.hpp"

#define MOVE_AMOUNT 16.0

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

// Shader Objects

struct VertexShader : public Topl_Shader {
	VertexShader(const char* filePath)
		: Topl_Shader(
			SHDR_Vertex, filePath,
			{ Shader_Type("pos", SHDR_float_vec3) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		const uint8_t* rotationBytes_ptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
		const uint8_t* offsetBytes_ptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());

		bytes->assign({ 
			*(rotationBytes_ptr + 0), *(rotationBytes_ptr + 1), *(rotationBytes_ptr + 2), *(rotationBytes_ptr + 3), // Main axis rotation
			*(rotationBytes_ptr + 4), *(rotationBytes_ptr + 5), *(rotationBytes_ptr + 6), *(rotationBytes_ptr + 7), // Cross axis rotation
			0, 0, 0, 0, // 0 byte padding
			0, 0, 0, 0, // 0 byte padding
			*(offsetBytes_ptr + 0), *(offsetBytes_ptr + 1), *(offsetBytes_ptr + 2), *(offsetBytes_ptr + 3), // X offset value
			*(offsetBytes_ptr + 4), *(offsetBytes_ptr + 5), *(offsetBytes_ptr + 6), *(offsetBytes_ptr + 7), // Y offset value
			*(offsetBytes_ptr + 8), *(offsetBytes_ptr + 9), *(offsetBytes_ptr + 10), *(offsetBytes_ptr + 11), // Z offset value
			0, 0, 0, 0 // 0 byte padding
		});

		return true; // Indicates that an implementation exists
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false;
	}
};

struct FragmentShader : public Topl_Shader {
	FragmentShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("flatColor", SHDR_uint) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		return false; // Indicates that an implementation is absent
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const {
		return false;
	}                                                                       
};