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
    // Fixed constructor for instantiating a fixed number of dissimilar geo components
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, std::initializer_list<Geo_RenderObj*> renderObjs) {
		mPrefix = prefix;
        mGeoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mGeoData + mGeoCount) = new Geo_Component(*(currentRenderObj));
            mGeoCount++;
        }
	}
    // Spam constructor for instantiating multiple duplicate geo components
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, const Geo_Component* geocLink, unsigned count) {
		mPrefix = prefix;
        // TODO: Populate mGeoData! Figure this crap out!
        /*mGeoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
        for(std::vector<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mGeoData + mGeoCount) = new Geo_Component(*(currentRenderObj));
            mGeoCount++;
        } */
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

// ------- Follow up with concrete implementations of Geo_Construct Objects -------- // 

#include "Humanoid.hpp"