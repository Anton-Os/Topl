#include <initializer_list>
#include <utility>
#include <cstdlib> // May be redundant
#include <vector> // May be redundant
#include <string>

#include "Geometry.hpp"
#include "Timer.hpp"

#include "Topl_SceneManager.hpp"

// Geometry wrapper class that can manage its states

typedef std::pair<std::string, Topl_GeoNode*> geoName_pair;

class Geo_Construct {
public:
    Geo_Construct(){ } // For more complex objects that interface directly with sceneManager
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, std::initializer_list<Geo_RenderObj*> renderObjs) {
		mNodeData = (Topl_GeoNode**)malloc(renderObjs.size() * sizeof(Topl_GeoNode*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mNodeData + mNodeCount) = new  Topl_GeoNode(*(currentRenderObj));
            mNodeCount++;
        }
        // fillSceneManager(sMan);
	}
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, Topl_GeoNode* rootNode) {
		// Implement logic for allocating mNodeData to size of children
	}
	~Geo_Construct() { // Precaution for custom geo objects
        if(mNamedNodes.size() != 0 && mNodeData != nullptr) { 
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
                sMan->addGeometry(currentGeo->first, currentGeo->second, Eigen::Vector3f(0.0, 0.4, 0.0));
        }
    }

	virtual void updateSceneManager(Topl_SceneManager* sMan) = 0;
protected:
	virtual void fill(Topl_SceneManager* sMan) = 0; // Job is to fill the mNamedNodes structure
    void addGeometry(const std::string& str, Topl_GeoNode* node){
        mNamedNodes.push_back(std::make_pair(str, node)); // Add the prefix relationship
    }
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
	Topl_GeoNode** mNodeData;

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
};

class Geo_Character1 : public Geo_Construct { // Consists of sprites
public:
	Geo_Character1(const std::string& prefix, Topl_SceneManager* sMan) : Geo_Construct() 
	{ fillSceneManager(sMan); }
	~Geo_Character1() {
		if (mHead_rect != nullptr) delete mHead_rect;
		if (mHead_gNode != nullptr) delete mHead_gNode;
	}
	
    void updateSceneManager(Topl_SceneManager* sMan) override;
private:
	void fill(Topl_SceneManager* sMan) override;

	// Geo_Rect2D *mTorso_rect, *mHead_rect, *mLeftArm_rect, *mRightArm_rect, *mLeftLeg_rect, *mRightLeg_rect;
	// Topl_GeoNode *mTorso_gNode, *mHead_gNode, *mLeftArm_gNode, *mRightArm_gNode, *mLeftLeg_gNode, *mRightLeg_gNode;
	Geo_Rect2D* mHead_rect;
	Topl_GeoNode* mHead_gNode;
};


Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos);

class Geo_RandShapes {
public:
    Geo_RandShapes(Topl_SceneManager* sceneGraph){
        mBox1 = new Geo_Rect2D(0.8f, 0.6f);
        mGeoNode1 = new Topl_GeoNode((Geo_RenderObj*)mBox1);

        mBox2 = new Geo_Rect2D(0.2f, 0.3f);
        mGeoNode2 = new Topl_GeoNode((Geo_RenderObj*)mBox2);
        mGeoNode2->updatePos(Eigen::Vector3f(0.6f, -0.55f, 0.0f));

		mSphere1 = new Geo_Sphere2D(0.2f, 20);
        mGeoNode3 = new Topl_GeoNode((Geo_RenderObj*)mSphere1);
        mGeoNode3->updatePos(Eigen::Vector3f(-0.2f, -0.4f, 0.0f));

        fillSceneManager(sceneGraph);
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
    void fillSceneManager(Topl_SceneManager* sceneGraph);
    
    Geo_Rect2D* mBox1;
    Geo_Rect2D* mBox2;
	Geo_Sphere2D* mSphere1;
    Topl_GeoNode* mGeoNode1;
    Topl_GeoNode* mGeoNode2;
    Topl_GeoNode* mGeoNode3;
    // Physics_MoveAbs mUpMovement = Physics_MoveAbs(&updatePosAbs);
};

void createQuadTree(Topl_SceneManager* sceneGraph, unsigned divs);
