#include "Platform.hpp"

#include "Topl_Scene.hpp"
#include "Topl_Renderer.hpp"

#include "Geo_Tree.hpp"
#include "trees/Humanoid.hpp"

#define SHIFT_AMOUNT 0.2
#define ROTATE_AMOUNT 0.3

#define VIEW_SPACE 3.0f
#define MOVE_SCALE 30

namespace App {
	Topl_Scene scene;
	Topl_Camera camera = Topl_Camera(PROJECTION_Ortho, SpatialBounds3D(VIEW_SPACE));
	Timer_Ticker gameTicker;

	std::string assetsPath = ASSETS_DIR;
	std::string imagePath = assetsPath + "images/";

	std::pair<Vec3f, Vec2f> orientations[HUMANOID_PARTS_COUNT] = { // shared orientations
		std::make_pair(Vec3f({ 0.0f, 0.08f * VIEW_SPACE, 0.0 }), Vec2f({ -1.0 * TOPL_HALF_PI, 0.0 })),
		std::make_pair(Vec3f({ 0.12f * VIEW_SPACE, -0.15f * VIEW_SPACE, 0.0 }), Vec2f({ 0.0, 0.0 })),
		std::make_pair(Vec3f({ -0.12f * VIEW_SPACE, -0.15f * VIEW_SPACE, 0.0 }), Vec2f({ -1.0 * TOPL_PI, 0.0 })),
		std::make_pair(Vec3f({ 0.0f, -0.05f * VIEW_SPACE, 0.0 }), Vec2f({ -1.0 * TOPL_HALF_PI, 0.0 })),
		std::make_pair(Vec3f({ 0.05f * VIEW_SPACE, -0.2f * VIEW_SPACE, 0.0 }), Vec2f({ 0.0, 0.0 })),
		std::make_pair(Vec3f({ -0.05f * VIEW_SPACE, -0.2f * VIEW_SPACE, 0.0 }), Vec2f({ -1.0 * TOPL_PI, 0.0 }))
	};

	std::string demonAssets[HUMANOID_PARTS_COUNT] = { 
		imagePath + "Demon-Head.png", 
		imagePath + "Demon-LeftWing.png", 
		imagePath + "Demon-RightWing.png", 
		imagePath + "Demon-Torso.png", 
		imagePath + "Demon-LeftLeg.png", 
		imagePath + "Demon-RightLeg.png" 
	};

	std::string angelAssets[HUMANOID_PARTS_COUNT] = {
		imagePath + "Angel-Head.png",
		imagePath + "Angel-LeftWing.png",
		imagePath + "Angel-RightWing.png",
		imagePath + "Angel-Torso.png",
		imagePath + "Angel-LeftLeg.png",
		imagePath + "Angel-RightLeg.png"
	};

	std::string ghostAssets[HUMANOID_PARTS_COUNT] = {
		imagePath + "Ghost-Head.png",
		imagePath + "Ghost-LeftArm.png",
		imagePath + "Ghost-RightArm.png",
		imagePath + "Ghost-Torso.png",
		imagePath + "Ghost-LeftLeg.png",
		imagePath + "Ghost-RightLeg.png"
	};

	// Geo_Humanoid2D demon("demon", &scene, orientations, 0.25f);
	Geo_Humanoid2D demon("demon", &scene, demonAssets, 0.25f);
	Geo_Humanoid2D angel("angel", &scene, angelAssets, 0.25f);
	Geo_Humanoid2D ghost("ghost", &scene, ghostAssets, 0.25f);

	Phys_Connector demonAnchor, angelAnchor;
	Vec3f demonOffset = Vec3f({ -1.5f, 0.0f, 0.0f });
	Vec3f angelOffset = Vec3f({ 1.5f, 0.0f, 0.0f });
	Vec3f ghostOffset = Vec3f({ 0.0, -0.5f, 0.0f });
}

void buttonCallback_w(void) { 
	App::demon.move(Vec3f({ 0.0f, SHIFT_AMOUNT, 0.0f } ));
	App::angel.move(Vec3f({ 0.0f, -1 * SHIFT_AMOUNT, 0.0f }));
}
void buttonCallback_a(void) { 
	App::demon.move(Vec3f({ -1 * SHIFT_AMOUNT, 0.0f, 0.0f }));
	App::angel.move(Vec3f({ SHIFT_AMOUNT, 0.0f, 0.0f }));
}
void buttonCallback_s(void) { 
	App::demon.move(Vec3f({ 0.0f, -1 * SHIFT_AMOUNT, 0.0f }));
	App::angel.move(Vec3f({ 0.0f, SHIFT_AMOUNT, 0.0f }));
}
void buttonCallback_d(void) { 
	App::demon.move(Vec3f({ SHIFT_AMOUNT, 0.0f, 0.0f }));
	App::angel.move(Vec3f({ -1 * SHIFT_AMOUNT, 0.0f, 0.0f }));
}
void buttonCallback_r(void) { 
	App::demon.rotateAll(Vec2f({ ROTATE_AMOUNT, ROTATE_AMOUNT }));
	App::ghost.rotate(Vec2f({ ROTATE_AMOUNT, 0.0f }));
	App::ghost.presetLinks();
}

void actionCallback() {
	// adding movements for testing
	App::scene.addForce("demon_head", Vec3f({ 0.0f, 3.0f * MOVE_SCALE, 0.0f }));
	App::scene.addForce("demon_body", Vec3f({ 0.0f, -3.0f * MOVE_SCALE, 0.0f }));
	App::scene.addForce("demon_rightLeg", Vec3f({ MOVE_SCALE, 0.0f, 0.0f }));
	App::scene.addForce("demon_leftLeg", Vec3f({ -1.0f * MOVE_SCALE, 0.0f, 0.0f }));

	App::scene.addForce("angel_head", Vec3f({ 0.0f, -1.0f * MOVE_SCALE, 0.0f }));
	App::scene.addForce("angel_body", Vec3f({ 0.0f, MOVE_SCALE, 0.0f }));
	App::scene.addForce("angel_rightLeg", Vec3f({ -2.0f, 4.0f * MOVE_SCALE, 0.0f }));
	App::scene.addForce("angel_leftLeg", Vec3f({ 2.0f, -4.0f * MOVE_SCALE, 0.0f }));
}

// Shared functions

namespace Main {
	void init(Platform* platform) {
		platform->createWindow();

		App::gameTicker.addPeriodicEvent(1000, actionCallback);

		Platform::keyControl.addCallback('w', buttonCallback_w);
		Platform::keyControl.addCallback('a', buttonCallback_a);
		Platform::keyControl.addCallback('s', buttonCallback_s);
		Platform::keyControl.addCallback('d', buttonCallback_d);
		Platform::keyControl.addCallback('r', buttonCallback_r);

		// Initial Positions
		App::demon.move(App::demonOffset);
		App::demon.orientAll(App::orientations);
		App::angel.move(App::angelOffset);
		App::angel.orientAll(App::orientations);
		App::ghost.move(App::ghostOffset);
		App::ghost.orientAll(App::orientations);

		App::scene.addAnchor(&App::demonAnchor, "demon_body", &App::demonOffset);
		App::scene.addAnchor(&App::angelAnchor, "angel_head", &App::angelOffset);
	}

	void gameLoop(Platform* platform, Topl_Renderer* renderer) {
		while (1) {
			App::gameTicker.updateTimer();
			App::scene.resolvePhysics();

			renderer->clearView();
			renderer->updateScene(&App::scene);
			renderer->renderScene(&App::scene);
			renderer->switchFramebuff();

			platform->handleEvents(false);
		}
	}
}