#include "Platform.hpp"

#include "Topl_Renderer_Drx11.hpp"

#include "Geo_Construct.hpp"
#include "primitives/Geo_Flat.hpp"
#include "primitives/Geo_Conic.hpp"
#include "primitives/Geo_Extruded.hpp"
#include "composites/Chain.hpp"
#include "composites/Grid.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	// Management Objects
	Topl_Scene scene;

	// Primitive Geometry Objects
	Geo_FlatHex hex1 = Geo_FlatHex(0.1f);
	Geo_ExtrudedSquare rect1 = Geo_ExtrudedSquare(0.1f, 0.4f);
	Geo_ConicSquare cone1 = Geo_ConicSquare(0.1f, Eigen::Vector3f(0.0f, 0.0f, 0.2f));
	// Complex Geometry Objects
	Geo_Component chainGeo = Geo_Component((const Geo_RenderObj*)&cone1);
	Geo_Chain_Properties chainProps = Geo_Chain_Properties(0.45f); // argument is the distance apart
	Geo_Chain chain("chain", &scene, &chainGeo, &chainProps, 4);
	Geo_Component gridGeo = Geo_Component((const Geo_RenderObj*)&rect1);
	Geo_Grid_Properties gridProps = Geo_Grid_Properties(std::make_pair(1, 0.5f));
	Geo_Grid grid("grid", &scene, &gridGeo, &gridProps);
}

#include "Drx11_Volumes.hpp"
#include "Drx11_Flat.hpp"

void buttonCallback_w(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right