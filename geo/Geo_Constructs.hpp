#include <initializer_list>
#include <utility>
#include <cstdlib> // May be redundant
#include <vector> // May be redundant
#include <string>

#include "Geometry.hpp" // Includes Physics.hpp as well
#include "Timer.hpp"

#include "Topl_SceneManager.hpp"

#ifdef RASTERON_H
	#include "Geo_SpriteTable.hpp"
#endif

// Geometry wrapper class that can manage its states

typedef std::pair<std::string, Topl_GeoNode*> geoName_pair;

class Geo_Construct {
public:
    Geo_Construct(){ } // For more complex objects that interface directly with sceneManager
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, std::initializer_list<Geo_RenderObj*> renderObjs) {
		mNodeData = (Topl_GeoNode**)malloc(renderObjs.size() * sizeof(Topl_GeoNode*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mNodeData + mNodeCount) = new Topl_GeoNode(*(currentRenderObj));
            mNodeCount++;
        }
        // fillSceneManager(sMan);
	}
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, Topl_GeoNode* rootNode) {
		// Implement logic for allocating mNodeData to size of children
	}
	~Geo_Construct() { // Precaution for custom geo objects, bad design friend
        if(mNodeData != nullptr) { 
            for (unsigned g = 0; g < mNodeCount; g++)
                delete *(mNodeData + g);
            free(mNodeData);
        }
    }
    void fillSceneManager(Topl_SceneManager* sMan){
        fill(sMan); // Calls virtual function

        if(mNamedNodes.size() != 0 && mNodeData != nullptr) { // Precaution for custom geo objects
            for(std::vector<geoName_pair>::iterator currentGeo = mNamedNodes.begin();
                currentGeo < mNamedNodes.end(); currentGeo++)
                sMan->addGeometry(currentGeo->first, currentGeo->second);
        }
    }

	virtual void updateSceneManager(Topl_SceneManager* sMan) = 0;
protected:
	virtual void fill(Topl_SceneManager* sMan) = 0; // Job is to fill the mNamedNodes structure
    Topl_GeoNode* getNextNode(){
        if(mCurrentNodeOffset <= mNodeCount){
            mCurrentNodeOffset++;   
            return *(mNodeData + mCurrentNodeOffset - 1); // To increment offset above in one line
        } else return nullptr;
    }

	std::vector<geoName_pair> mNamedNodes;
private:
	unsigned mNodeCount = 0;
    unsigned mCurrentNodeOffset = 0;
	Topl_GeoNode** mNodeData = nullptr;

}; // Needs work, could use more abstraction

class Geo_CircleUp : public Geo_Construct {
public:
	Geo_CircleUp(const std::string& prefix, Topl_SceneManager* sMan) : 
    Geo_Construct(prefix, sMan, { (Geo_RenderObj*)&sphere1, (Geo_RenderObj*)&sphere2, (Geo_RenderObj*)&sphere3 }) 
		{ fillSceneManager(sMan); }
	
    void updateSceneManager(Topl_SceneManager* sMan) override;
private:
	void fill(Topl_SceneManager* sMan) override;

	Geo_Sphere2D sphere1 = Geo_Sphere2D(0.1f, 4);
	Geo_Sphere2D sphere2 = Geo_Sphere2D(0.4f, 200);
	Geo_Sphere2D sphere3 = Geo_Sphere2D(0.333f, 42);

    Phys_Properties mPhysProp1; // Initialized to defaults;
    Phys_Properties mPhysProp2; // Initialized to defaults;
    Phys_Properties mPhysProp3; // Initialized to defaults;
};

#ifdef RASTERON_H
class Geo_Character1 : protected Geo_SpriteTable, public Geo_Construct { // Consists of sprites
public:
	Geo_Character1(const std::string& prefix, Topl_SceneManager* sMan) :
	Geo_SpriteTable({ 
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Body.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftArm.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightArm.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\LeftLeg.png",
		"C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\RightLeg.png",
	}),
	Geo_Construct(prefix, sMan, { 
		(Geo_RenderObj*)getRect(CHAR_Head),
		(Geo_RenderObj*)getRect(CHAR_Body),
		(Geo_RenderObj*)getRect(CHAR_LeftArm),
		(Geo_RenderObj*)getRect(CHAR_RightArm),
		(Geo_RenderObj*)getRect(CHAR_LeftLeg),
		(Geo_RenderObj*)getRect(CHAR_RightLeg),
	}) // Inherited from Sprite table
	{ fillSceneManager(sMan); }

	~Geo_Character1() {}
	
	enum CHAR_Anatomy {
		CHAR_Head = 0,
		CHAR_Body = 1,
		CHAR_LeftArm = 2,
		CHAR_RightArm = 3,
		CHAR_LeftLeg = 4,
		CHAR_RightLeg = 5
	} anatomyIndex;

    void updateSceneManager(Topl_SceneManager* sMan) override;
private:
	void fill(Topl_SceneManager* sMan) override;
};
#endif


Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos);

class Geo_RandShapes {
public:
    Geo_RandShapes(Topl_SceneManager* sMan){
        mBox1 = new Geo_Rect2D(0.8f, 0.6f);
        mGeoNode1 = new Topl_GeoNode((Geo_RenderObj*)mBox1);

        mBox2 = new Geo_Rect2D(0.2f, 0.3f);
        mGeoNode2 = new Topl_GeoNode((Geo_RenderObj*)mBox2);
        mGeoNode2->updatePos(Eigen::Vector3f(0.6f, -0.55f, 0.0f));

		mSphere1 = new Geo_Sphere2D(0.2f, 20);
        mGeoNode3 = new Topl_GeoNode((Geo_RenderObj*)mSphere1);
        mGeoNode3->updatePos(Eigen::Vector3f(-0.2f, -0.4f, 0.0f));

        fillSceneManager(sMan);
    }
    ~Geo_RandShapes(){
        delete mBox1;
        delete mGeoNode1;
        delete mBox2;
        delete mGeoNode2;
        delete mSphere1;
        delete mGeoNode3;
    }
    // void updateSceneManager(Topl_SceneManager* Topl_SceneManager, Timer_DiscreteEvent timerEvent); // Should probably make private
private:
    void fillSceneManager(Topl_SceneManager* sMan);
    
    Geo_Rect2D* mBox1;
    Geo_Rect2D* mBox2;
	Geo_Sphere2D* mSphere1;
    Topl_GeoNode* mGeoNode1;
    Topl_GeoNode* mGeoNode2;
    Topl_GeoNode* mGeoNode3;
    // Physics_MoveAbs mUpMovement = Physics_MoveAbs(&updatePosAbs);
};

void createQuadTree(Topl_SceneManager* sMan, unsigned divs);
