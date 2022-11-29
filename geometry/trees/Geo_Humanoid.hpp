#include "ShapesGen.hpp"

#include "Geo_Tree.hpp"
#ifdef TOPL_ENABLE_MODEL
	#include "Geo_Model.hpp"
#endif

#include "primitives/Geo_Flat.hpp"

// Base Humanoid

#define HUMANOID_PARTS_COUNT 6 // There are 6 torso parts total

enum HUMANOID_Anatomy {
	HUMANOID_Head = 0,
	HUMANOID_LeftArm = 1,
	HUMANOID_RightArm = 2,
	HUMANOID_Torso = 3,
	HUMANOID_LeftLeg = 4,
	HUMANOID_RightLeg = 5
};

const std::string bodyPartsStr[HUMANOID_PARTS_COUNT] = {
	"head", "leftArm", "rightArm", "torso", "leftLeg", "rightLeg"
};

class Geo_Humanoid {
public:
	void orient(HUMANOID_Anatomy target, const Vec3f& pos, const Vec3f& angles); // orients torso parts
	void orientAll(std::pair<Vec3f, Vec3f> orientations[HUMANOID_PARTS_COUNT]); // orients all torso parts

	void addLinks(Topl_Scene* scene, const std::string& prefix);
	void presetLinks();
protected:
	Geo_Humanoid(){}
	// Geometry Actors
	Geo_Actor *head, *leftArm, *rightArm, *torso, *leftLeg, *rightLeg;
	Geo_Actor** bodyActors[HUMANOID_PARTS_COUNT] = { &head, &leftArm, &rightArm, &torso, &leftLeg, &rightLeg };
	// Physics Properties
    Phys_Actor head_phys, leftArm_phys, rightArm_phys, torso_phys, leftLeg_phys, rightLeg_phys;
	Phys_Actor* bodyPhysics[HUMANOID_PARTS_COUNT] = { &head_phys, &leftArm_phys, &rightArm_phys, &torso_phys, &leftLeg_phys, &rightLeg_phys };
	// Starfish shaped links from body
	Phys_Connector torso_head_link, torso_leftArm_link, torso_rightArm_link, torso_leftLeg_link, torso_rightLeg_link;
	// Pentagon shaped links surrounding body
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
		(Geo_Renderable*)_renderObjs[HUMANOID_Head], 
		(Geo_Renderable*)_renderObjs[HUMANOID_LeftArm],
		(Geo_Renderable*)_renderObjs[HUMANOID_RightArm],
		(Geo_Renderable*)_renderObjs[HUMANOID_Torso],
		(Geo_Renderable*)_renderObjs[HUMANOID_LeftLeg], 
		(Geo_Renderable*)_renderObjs[HUMANOID_RightLeg]
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
		(Geo_Renderable*)_renderObjs[HUMANOID_Head], 
		(Geo_Renderable*)_renderObjs[HUMANOID_LeftArm], 
		(Geo_Renderable*)_renderObjs[HUMANOID_RightArm],
		(Geo_Renderable*)_renderObjs[HUMANOID_Torso],
		(Geo_Renderable*)_renderObjs[HUMANOID_LeftLeg], 
		(Geo_Renderable*)_renderObjs[HUMANOID_RightLeg]
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

#ifdef TOPL_ENABLE_MODEL

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

#endif