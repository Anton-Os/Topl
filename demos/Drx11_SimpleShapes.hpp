#include "native_os_def.h"

#include "FileIO.hpp"
// #include "Input.hpp"
#include "Platform.hpp"

#include "Topl_Renderer_Drx11.hpp"

#include "Geo_Construct.hpp"
#include "primitives/Geo_Sphere2D.hpp"
#include "primitives/Geo_Rect3D.hpp"
#include "composites/Chain.hpp"
#include "composites/Grid.hpp"

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
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3) } // Inputs
		) {  }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override {
		bytes->clear(); // Make sure there is no preexisting data

		const uint8_t* offsetBytes_ptr = reinterpret_cast<const uint8_t*>(component->getPos()->data());
		const uint8_t* rotationBytes_ptr = reinterpret_cast<const uint8_t*>(component->getAngles()->data());
	
		ValueGen::appendDataToBytes(offsetBytes_ptr, component->getPos()->size() * sizeof(float), 1 * sizeof(float), bytes);
		ValueGen::appendDataToBytes(rotationBytes_ptr, component->getAngles()->size() * sizeof(float), 2 * sizeof(float), bytes);
		return true;
	}

	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const { return false; }
};

struct PixelShader : public Topl_Shader {
	PixelShader(const char* filePath)
		: Topl_Shader(
			SHDR_Fragment, filePath,
			{ Shader_Type("pos", "POSITION", SHDR_float_vec3), Shader_Type("uint", "COLOR0", SHDR_uint) } // Inputs
		) { }

	virtual bool genPerGeoDataBlock(const Geo_Component* const component, std::vector<uint8_t>* bytes) const override { return false; }
	virtual bool genPerSceneDataBlock(const Topl_SceneManager* const sMan, std::vector<uint8_t>* bytes) const { return false; }
};
