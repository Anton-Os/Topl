#ifdef RASTERON_H

// TODO: Reinclude code below when header is on its own
// #include "Geo_Constructs.hpp"
// #include "Geo_SpriteTable.hpp"

class Geo_Humanoid : protected Geo_SpriteTable, public Geo_Construct { // Consists of sprites
public:
	// Easy preconfigured constructor
	Geo_Humanoid(const std::string& prefix, Topl_SceneManager* sMan) :
	Geo_SpriteTable({ 
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftArm.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightArm.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Body.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftLeg.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightLeg.png"
	    }, 0.2f // Scaling the sprites to 20% original size
    ),
	Geo_Construct(prefix, sMan, { 
		// getRect is inherited from the SpriteTable class, might rename the method for clarity
		// HUMANOID_Head, HUMANOID_LeftArm, etc. are simply indexes 1 to 6 for proper loading
		(Geo_RenderObj*)getRect(HUMANOID_Head),
		(Geo_RenderObj*)getRect(HUMANOID_LeftArm),
		(Geo_RenderObj*)getRect(HUMANOID_RightArm),
		(Geo_RenderObj*)getRect(HUMANOID_Body),
		(Geo_RenderObj*)getRect(HUMANOID_LeftLeg),
		(Geo_RenderObj*)getRect(HUMANOID_RightLeg), }
	) { 
		// Populated for Urkwin Alien with predefined values
		headOffset = Eigen::Vector3f(0.0f, 0.11f, 0.0);
		bodyOffset = Eigen::Vector3f(0.0f, -0.1f, 0.0);
		leftArmOffset = Eigen::Vector3f(0.12f, -0.14f, 0.0);
		rightArmOffset = Eigen::Vector3f(-0.12f, -0.14f, 0.0);
		leftLegOffset = Eigen::Vector3f(0.06f, -0.35f, 0.0);
		rightLegOffset = Eigen::Vector3f(-0.06f, -0.35f, 0.0);

		fillSceneManager(sMan); 
	}

	/* // Extensive configurable constructor
	Geo_Humanoid(
		const std::string& prefix, 
		Topl_SceneManager* sMan, 
		std::initializer_list<std::pair<const char*, Eigen::Vector3f>> properties,
		float scaleFactor)
	: Geo_SpriteTable({}, scaleFactor),
	Geo_Construct(prefix, sMan, { }) {

		// TODO: Implement body here!
		fillSceneManager(sMan);
	} */

	~Geo_Humanoid() {}
	
	enum HUMANOID_Anatomy {
		HUMANOID_Head = 0,
		HUMANOID_LeftArm = 1,
		HUMANOID_RightArm = 2,
		HUMANOID_Body = 3,
		HUMANOID_LeftLeg = 4,
		HUMANOID_RightLeg = 5
	} anatomyIndex;

    void updateSceneManager(Topl_SceneManager* sMan) override;
	// Moves the object in the given direction, TODO: Make this a base class virtual function
	void move(Topl_SceneManager* sMan, Eigen::Vector3f vec);
private:
	void fill(Topl_SceneManager* sMan) override;

	// Main links "starfish"
	Phys_Connector body_head_link, body_leftArm_link, body_rightArm_link, body_leftLeg_link, body_rightLeg_link;
	// Stability links "pentagon"
	Phys_Connector head_leftArm_link, head_rightArm_link, leftArm_leftLeg_link, rightArm_rightLeg_link, leftLeg_rightLeg_link;

	// Offsets associated with each body part to positon them appropriately
	Eigen::Vector3f headOffset, bodyOffset, rightArmOffset, leftArmOffset, rightLegOffset, leftLegOffset;
	// Physics properties associated with each body part
    Phys_Properties head_phys, body_phys, leftArm_phys, rightArm_phys, leftLeg_phys, rightLeg_phys;
};

#endif