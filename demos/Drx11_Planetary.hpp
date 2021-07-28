#include "Platform.hpp"

#include "Topl_Renderer_Drx11.hpp"

#include "primitives/Geo_Sphere.hpp"

#define MOVE_AMOUNT 0.5

namespace Topl {
	Topl_Scene scene;

	NGon3D ngon = { 0.2, 20, 30 };
	Geo_SphereUV sphereGeo(ngon);
	Geo_Component sphere((const Geo_RenderObj*)&sphereGeo);
}

#include "Drx11_Volumes.hpp"
#include "Drx11_Flat.hpp"

void buttonCallback_w(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, MOVE_AMOUNT)); } // Move forward
void buttonCallback_a(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(-1.0f * MOVE_AMOUNT, 0.0f, 0.0)); } // Move left
void buttonCallback_s(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(0.0f, 0.0f, -1.0f * MOVE_AMOUNT)); } // Move backwards
void buttonCallback_d(void) { Topl::scene.moveCameraPos(Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right