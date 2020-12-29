#include <initializer_list>
#include <utility>
#include <cstdlib> // May be redundant
#include <vector> // May be redundant
#include <string>

#include "Geometry.hpp" // Includes Physics.hpp as well
#include "Timer.hpp"

#include "Topl_SceneManager.hpp"

// Geometry wrapper class that can manage its states

#define FIRST_UPDATE_NUM 0

typedef std::pair<std::string, Geo_Component*> geoName_pair;

// TODO: Node data must be changed to geo data
class Geo_Construct {
public:
    // Fixed items constructor
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, std::initializer_list<Geo_RenderObj*> renderObjs) {
		mPrefix = prefix;
        mGeoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mGeoData + mGeoCount) = new Geo_Component(*(currentRenderObj));
            mGeoCount++;
        }
	}
    // Duplicate items constructor
    Geo_Construct(const std::string& prefix, Topl_SceneManager* sMan, const Geo_Component* geoc, unsigned count) {
		mPrefix = prefix;
        mGeoCount = count;
        mGeoData = (Geo_Component**)malloc(count * sizeof(Geo_Component));

        for(unsigned g = 0; g < count; g++) *(mGeoData + g) = new Geo_Component(*geoc);
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
    virtual void move(Topl_SceneManager* sMan, Eigen::Vector3f vec) = 0;
    virtual void rotate(Topl_SceneManager* sMan, Eigen::Vector3f) = 0;

protected:
	unsigned getGeoCount(){ return mGeoCount; }
    virtual void fill(Topl_SceneManager* sMan) = 0; // Job is to fill the mNamedGeos structure
    Geo_Component* getNextGeo(){
        if(mCurrentGeoOffset <= mGeoCount){
            mCurrentGeoOffset++;   
            return *(mGeoData + mCurrentGeoOffset - 1); // To increment offset above in one line
        } else return nullptr;
    }

    unsigned mUpdateCount = FIRST_UPDATE_NUM; // Probably needs to be private and a getter method
	std::vector<geoName_pair> mNamedGeos;
private:
    std::string mPrefix;
	unsigned mGeoCount = 0;
    unsigned mCurrentGeoOffset = 0;
	Geo_Component** mGeoData = nullptr;
}; // Needs work, could use more abstraction