#include "Geo_Construct.hpp"
#include "Geo_SpriteTable.hpp"

#define HUMANOID_PARTS_COUNT 6 // There are 6 body parts total

class Geo_Humanoid : protected Geo_SpriteTable, public Geo_Construct { // Consists of sprites
public:
	Geo_Humanoid( // Customizable constructor
		const std::string& prefix,
		Topl_Scene* scene,
		std::pair<const char*, Eigen::Vector3f> initProps[HUMANOID_PARTS_COUNT], // Filepaths and offsets for
		float scaleFactor )
		
		: Geo_SpriteTable({
			initProps[HUMANOID_Head].first,
			initProps[HUMANOID_LeftArm].first,
			initProps[HUMANOID_RightArm].first,
			initProps[HUMANOID_Body].first,
			initProps[HUMANOID_LeftLeg].first,
			initProps[HUMANOID_RightLeg].first
			}, scaleFactor
		),
		Geo_Construct(prefix, scene, {
			(Geo_RenderObj*)getRect(HUMANOID_Head),
			(Geo_RenderObj*)getRect(HUMANOID_LeftArm),
			(Geo_RenderObj*)getRect(HUMANOID_RightArm),
			(Geo_RenderObj*)getRect(HUMANOID_Body),
			(Geo_RenderObj*)getRect(HUMANOID_LeftLeg),
			(Geo_RenderObj*)getRect(HUMANOID_RightLeg), }
		) {

		headOffset = initProps[HUMANOID_Head].second;
		bodyOffset = initProps[HUMANOID_LeftArm].second;
		leftArmOffset = initProps[HUMANOID_RightArm].second;
		rightArmOffset = initProps[HUMANOID_Body].second;
		leftLegOffset = initProps[HUMANOID_LeftLeg].second;
		rightLegOffset = initProps[HUMANOID_RightLeg].second;

		fillSceneManager(scene);
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

    void updateSceneManager(Topl_Scene* scene) override;
private:
	void fill(Topl_Scene* scene) override;

	// Main links "starfish"
	Phys_Connector body_head_link, body_leftArm_link, body_rightArm_link, body_leftLeg_link, body_rightLeg_link;
	// Stability links "pentagon"
	Phys_Connector head_leftArm_link, head_rightArm_link, leftArm_leftLeg_link, rightArm_rightLeg_link, leftLeg_rightLeg_link;
	// Offsets associated with each body part to positon them appropriately
	Eigen::Vector3f headOffset, bodyOffset, rightArmOffset, leftArmOffset, rightLegOffset, leftLegOffset;
	// Physics properties associated with each body part
    Phys_Properties head_phys, body_phys, leftArm_phys, rightArm_phys, leftLeg_phys, rightLeg_phys;
};