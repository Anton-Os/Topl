#include "Geo_Troupe.hpp"
#include "Geo_SpriteTable.hpp"

#define HUMANOID_PARTS_COUNT 6 // There are 6 body parts total

// TODO: Make this a derived class of Web.hpp !!!

class Geo_Humanoid : protected Geo_SpriteTable, public Geo_Troupe { // Consists of sprites
public:
	Geo_Humanoid( // Customizable constructor
		const std::string& prefix,
		Topl_Scene* scene,
		std::pair<std::string, Eigen::Vector3f> props[HUMANOID_PARTS_COUNT], // humanoid property list of sprite images and positions
		float scaleFactor 
	)
		
	: Geo_SpriteTable({
		props[HUMANOID_Head].first,
		props[HUMANOID_LeftArm].first,
		props[HUMANOID_RightArm].first,
		props[HUMANOID_Body].first,
		props[HUMANOID_LeftLeg].first,
		props[HUMANOID_RightLeg].first
		}, scaleFactor
	),
	Geo_Troupe(prefix, scene, {
		(Geo_RenderObj*)getSquare(HUMANOID_Head),
		(Geo_RenderObj*)getSquare(HUMANOID_LeftArm),
		(Geo_RenderObj*)getSquare(HUMANOID_RightArm),
		(Geo_RenderObj*)getSquare(HUMANOID_Body),
		(Geo_RenderObj*)getSquare(HUMANOID_LeftLeg),
		(Geo_RenderObj*)getSquare(HUMANOID_RightLeg), }
	) {

		headOffset = props[HUMANOID_Head].second;
		bodyOffset = props[HUMANOID_LeftArm].second;
		leftArmOffset = props[HUMANOID_RightArm].second;
		rightArmOffset = props[HUMANOID_Body].second;
		leftLegOffset = props[HUMANOID_LeftLeg].second;
		rightLegOffset = props[HUMANOID_RightLeg].second;

		fillScene(scene);
	}
	~Geo_Humanoid() {}
	
	enum HUMANOID_Anatomy {
		HUMANOID_Head = 0,
		HUMANOID_LeftArm = 1,
		HUMANOID_RightArm = 2,
		HUMANOID_Body = 3,
		HUMANOID_LeftLeg = 4,
		HUMANOID_RightLeg = 5
	} anatomyIndex;

    void updateScene(Topl_Scene* scene) override;
private:
	void fill(Topl_Scene* scene) override;

	// Offsets associated with each body part to positon them appropriately
	Eigen::Vector3f headOffset, bodyOffset, rightArmOffset, leftArmOffset, rightLegOffset, leftLegOffset;
	// Physics properties associated with each body part
    Phys_Actor head_phys, body_phys, leftArm_phys, rightArm_phys, leftLeg_phys, rightLeg_phys;
	// Main links "Starfish" Shape
	Phys_Connector body_head_link, body_leftArm_link, body_rightArm_link, body_leftLeg_link, body_rightLeg_link;
	// Stability links "Pentagon" Shape
	Phys_Connector head_leftArm_link, head_rightArm_link, leftArm_leftLeg_link, rightArm_rightLeg_link, leftLeg_rightLeg_link;
};