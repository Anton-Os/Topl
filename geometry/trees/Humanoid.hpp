#include "Geo_Tree.hpp"
#include "Geo_SpriteTable.hpp"
// #include "Model.hpp"

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
	void orient(HUMANOID_Anatomy target, const Eigen::Vector3f& pos, const Eigen::Vector2f& angles); // orients body parts
	void orientAll(std::pair<Eigen::Vector3f, Eigen::Vector2f> orientations[HUMANOID_PARTS_COUNT]); // orients all body parts
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

class Geo_Humanoid2D : public Geo_Humanoid, protected Geo_SpriteTable, public Geo_Tree { // Consists of sprites
public:
	Geo_Humanoid2D( // Assets constructor
		const std::string& prefix,
		Topl_Scene* scene,
		std::string assets[HUMANOID_PARTS_COUNT], // image paths
		float scaleFactor 
	)
	: Geo_Humanoid(),
	Geo_SpriteTable({
		assets[HUMANOID_Head],
		assets[HUMANOID_LeftArm],
		assets[HUMANOID_RightArm],
		assets[HUMANOID_Body],
		assets[HUMANOID_LeftLeg],
		assets[HUMANOID_RightLeg]
		}, scaleFactor
	),
	Geo_Tree(prefix, {
		(Geo_RenderObj*)getSquare(HUMANOID_Head),
		(Geo_RenderObj*)getSquare(HUMANOID_LeftArm),
		(Geo_RenderObj*)getSquare(HUMANOID_RightArm),
		(Geo_RenderObj*)getSquare(HUMANOID_Body),
		(Geo_RenderObj*)getSquare(HUMANOID_LeftLeg),
		(Geo_RenderObj*)getSquare(HUMANOID_RightLeg), }
	) 
	{ init(scene); }
	
	~Geo_Humanoid2D() {}

	void init(Topl_Scene* scene) override;
};

/* class Geo_Humanoid3D : public Geo_Humanoid, public Geo_Model {
public:
	Geo_Model(
		const std::string& prefix,
        const std::string& filePath,
        Topl_Scene* scene
    )
	: Geo_Humanoid(),
	: Geo_Model(prefix, filePath, scene){
		// init(scene);
	}

	// void init(Topl_Scene* scene) override;
}; */