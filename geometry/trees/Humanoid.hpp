#include "ShapesGen.hpp"

#include "Geo_Tree.hpp"
#include "Model.hpp"

#include "primitives/Geo_Flat.hpp"

// Base Humanoid

#define HUMANOID_PARTS_COUNT 6 // There are 6 body parts total

enum HUMANOID_Anatomy {
	HUMANOID_Head = 0,
	HUMANOID_LeftArm = 1,
	HUMANOID_RightArm = 2,
	HUMANOID_Body = 3,
	HUMANOID_LeftLeg = 4,
	HUMANOID_RightLeg = 5
};

class Geo_Humanoid {
public:
	void orient(HUMANOID_Anatomy target, const Vec3f& pos, const Vec2f& angles); // orients body parts
	void orientAll(std::pair<Vec3f, Vec2f> orientations[HUMANOID_PARTS_COUNT]); // orients all body parts

	void presetLinks();
protected:
	Geo_Humanoid(){}
	// Geometry Actors
	Geo_Actor *head, *leftArm, *rightArm, *body, *leftLeg, *rightLeg;
	// Physics properties associated with each body part
    Phys_Actor head_phys, body_phys, leftArm_phys, rightArm_phys, leftLeg_phys, rightLeg_phys;
	// Main links "Starfish" Shape
	Phys_Connector body_head_link, body_leftArm_link, body_rightArm_link, body_leftLeg_link, body_rightLeg_link;
	// Stability links "Pentagon" Shape
	Phys_Connector head_leftArm_link, head_rightArm_link, leftArm_leftLeg_link, rightArm_rightLeg_link, leftLeg_rightLeg_link;
};

// 2D Sprite Humanoid

class Geo_Humanoid2D : public Geo_Humanoid, public Geo_Tree { // Consists of sprites
public:
	Geo_Humanoid2D( // Prebake Constructor
		const std::string& prefix,
		std::string assetPaths[HUMANOID_PARTS_COUNT] // image paths
		// float scaleFactor 
	)
	: Geo_Humanoid(),
	Geo_Tree(prefix, {
		(Geo_RenderObj*)_renderObjs[HUMANOID_Head], 
		(Geo_RenderObj*)_renderObjs[HUMANOID_LeftArm],
		(Geo_RenderObj*)_renderObjs[HUMANOID_RightArm],
		(Geo_RenderObj*)_renderObjs[HUMANOID_Body],
		(Geo_RenderObj*)_renderObjs[HUMANOID_LeftLeg], 
		(Geo_RenderObj*)_renderObjs[HUMANOID_RightLeg]
	}) {
		for (unsigned a = 0; a < HUMANOID_PARTS_COUNT; a++) _assetPaths[a] = assetPaths[a]; // copy over assets
	}

	Geo_Humanoid2D( // Config Constructor
		const std::string& prefix,
		Topl_Scene* scene,
		std::string assetPaths[HUMANOID_PARTS_COUNT] // image paths
		// float scaleFactor 
	)
	: Geo_Humanoid(),
	Geo_Tree(prefix, {
		(Geo_RenderObj*)_renderObjs[HUMANOID_Head], 
		(Geo_RenderObj*)_renderObjs[HUMANOID_LeftArm], 
		(Geo_RenderObj*)_renderObjs[HUMANOID_RightArm],
		(Geo_RenderObj*)_renderObjs[HUMANOID_Body],
		(Geo_RenderObj*)_renderObjs[HUMANOID_LeftLeg], 
		(Geo_RenderObj*)_renderObjs[HUMANOID_RightLeg]
	}) {
		for (unsigned a = 0; a < HUMANOID_PARTS_COUNT; a++) _assetPaths[a] = assetPaths[a]; // copy over assets
		configure(scene); 
	}

	~Geo_Humanoid2D();

	void configure(Topl_Scene* scene) override;

	std::string _assetPaths[HUMANOID_PARTS_COUNT];
	Rasteron_Sprite* _sprites[HUMANOID_PARTS_COUNT];
	Geo_FlatSquare* _renderObjs[HUMANOID_PARTS_COUNT];
};

// 3D Model Humanoid

class Geo_Humanoid3D : public Geo_Humanoid, public Geo_Model {
public:
	Geo_Humanoid3D(
		const std::string& prefix,
		const std::string& filePath
	) : Geo_Humanoid(),
	Geo_Model(prefix, filePath) {}

	Geo_Humanoid3D(
		const std::string& prefix,
        const std::string& filePath,
        Topl_Scene* scene
    )
	: Geo_Humanoid(),
	Geo_Model(prefix, filePath, scene){
		configure(scene);
	}

	void configure(Topl_Scene* scene) override;
};