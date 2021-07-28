#include "Platform.hpp"

#include "Topl_Renderer_GL4.hpp"

#include "Geo_Construct.hpp"
#include "composites/Humanoid.hpp"

#define MOVE_AMOUNT 16.0

namespace Topl {
	Topl_Scene scene;
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
	Geo_Humanoid humanoid("humanoid", &scene, humanoidProps, 0.25f);
}

#include "GL4_Basic.hpp" // Shader inclusion

void buttonCallback_w(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
void buttonCallback_a(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(-1 * MOVE_AMOUNT, 0.0f, 0.0f)); } // Move left
void buttonCallback_s(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(0.0f, -1 * MOVE_AMOUNT, 0.0f)); } // Move down
void buttonCallback_d(void) { Topl::humanoid.move(&Topl::scene, Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right
void buttonCallback_r(void) { Topl::humanoid.rotate(&Topl::scene, Eigen::Vector2f(1.0f, 0.0f)); } // Rotate