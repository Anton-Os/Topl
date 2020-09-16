#ifdef RASTERON_H

// TODO: Reinclude code below when header is on its own
// #include "Geo_Constructs.hpp"
// #include "Geo_SpriteTable.hpp"

class Geo_Humanoid : protected Geo_SpriteTable, public Geo_Construct { // Consists of sprites
public:
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
		(Geo_RenderObj*)getRect(HUMANOID_Head),
		(Geo_RenderObj*)getRect(HUMANOID_LeftArm),
		(Geo_RenderObj*)getRect(HUMANOID_RightArm),
		(Geo_RenderObj*)getRect(HUMANOID_Body),
		(Geo_RenderObj*)getRect(HUMANOID_LeftLeg),
		(Geo_RenderObj*)getRect(HUMANOID_RightLeg),
	}) // Inherited from Sprite table
	{ fillSceneManager(sMan); }

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

	Phys_Connector body_head_link, body_leftArm_link, body_rightArm_link, body_leftLeg_link, body_rightLeg_link; // Links to all relevant body parts
	Phys_Connector head_leftArm_link, head_rightArm_link, leftArm_leftLeg_link, rightArm_rightLeg_link, leftLeg_rightLeg_link; // Stability links

    Phys_Properties head_phys, body_phys, leftArm_phys, rightArm_phys, leftLeg_phys, rightLeg_phys; // Physics properties for all relevant body parts
};

#endif