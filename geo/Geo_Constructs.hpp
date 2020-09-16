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

#define FIRST_UPDATE_NUM 0

typedef std::pair<std::string, Geo_Component*> geoName_pair;

// TODO: Node data must be changed to geo data
class Geo_Construct {
public:
    Geo_Construct(){ } // For more complex objects that interface directly with sceneManager
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, std::initializer_list<Geo_RenderObj*> renderObjs) {
		mGeoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mGeoData + mGeoCount) = new Geo_Component(*(currentRenderObj));
            mGeoCount++;
        }
	}
	~Geo_Construct() { // Precaution for custom geo objects, bad design friend
        if(mGeoData != nullptr) { 
            for (unsigned g = 0; g < mGeoCount; g++)
                delete *(mGeoData + g);
            free(mGeoData);
        }
    }
    
    void fillSceneManager(Topl_SceneManager* sMan){
        fill(sMan); // Calls virtual function

        if(mNamedGeos.size() != 0 && mGeoData != nullptr) { // Precaution for custom geo objects
            for(std::vector<geoName_pair>::iterator currentGeo = mNamedGeos.begin();
                currentGeo < mNamedGeos.end(); currentGeo++)
                sMan->addGeometry(currentGeo->first, currentGeo->second);
        }
    }

    std::string getPrefix(){ return mPrefix + "_"; }
	virtual void updateSceneManager(Topl_SceneManager* sMan) = 0;
protected:
	virtual void fill(Topl_SceneManager* sMan) = 0; // Job is to fill the mNamedGeos structure
    Geo_Component* getNextGeo(){
        if(mCurrentGeoOffset <= mGeoCount){
            mCurrentGeoOffset++;   
            return *(mGeoData + mCurrentGeoOffset - 1); // To increment offset above in one line
        } else return nullptr;
    }

	std::vector<geoName_pair> mNamedGeos;
	unsigned mUpdateCount = FIRST_UPDATE_NUM; // Probably needs to be private and a getter method
private:
    std::string mPrefix;

	unsigned mGeoCount = 0;
    unsigned mCurrentGeoOffset = 0;
	Geo_Component** mGeoData = nullptr;
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

#include "Humanoid.hpp"

Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos);

class Geo_RandShapes {
public:
    Geo_RandShapes(Topl_SceneManager* sMan){
        mBox1 = new Geo_Rect2D(0.8f, 0.6f);
        mGeo1 = new Geo_Component((Geo_RenderObj*)mBox1);

        mBox2 = new Geo_Rect2D(0.2f, 0.3f);
        mGeo2 = new Geo_Component((Geo_RenderObj*)mBox2);
        mGeo2->updatePos(Eigen::Vector3f(0.6f, -0.55f, 0.0f));

		mSphere1 = new Geo_Sphere2D(0.2f, 20);
        mGeo3 = new Geo_Component((Geo_RenderObj*)mSphere1);
        mGeo3->updatePos(Eigen::Vector3f(-0.2f, -0.4f, 0.0f));

        fillSceneManager(sMan);
    }
    ~Geo_RandShapes(){
        delete mBox1;
        delete mGeo1;
        delete mBox2;
        delete mGeo2;
        delete mSphere1;
        delete mGeo3;
    }
    // void updateSceneManager(Topl_SceneManager* Topl_SceneManager, Timer_DiscreteEvent timerEvent); // Should probably make private
private:
    void fillSceneManager(Topl_SceneManager* sMan);
    
    Geo_Rect2D* mBox1;
    Geo_Rect2D* mBox2;
	Geo_Sphere2D* mSphere1;
    Geo_Component* mGeo1;
    Geo_Component* mGeo2;
    Geo_Component* mGeo3;
    // Physics_MoveAbs mUpMovement = Physics_MoveAbs(&updatePosAbs);
};